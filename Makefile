# Every command runs from the repo root as `make <target>`; `make help` lists
# the targets.

# Recipes run in bash for its `time` keyword, which prints TIMEFORMAT even when
# the timed command fails. $@ expands per target, so every timed recipe prints
# `timing <target> <seconds>s`.
SHELL := /bin/bash
export TIMEFORMAT = timing $@ %Rs

.DEFAULT_GOAL := firmware

##@ Firmware

# The QMK userspace is src/. QMK looks for qmk.json in the current directory
# and its parents, then in QMK_USERSPACE, then in user.overlay_dir, so from the
# repo root only QMK_USERSPACE points it at this checkout's src/.
export QMK_USERSPACE := $(CURDIR)/src

# Number of make jobs in QMK's firmware build, one per CPU by default. QMK
# itself runs one job unless told otherwise. `make QMK_JOBS=1` builds serially.
# Only QMK's make runs in parallel, never this make's own targets.
QMK_JOBS ?= $(shell nproc)

# QMK copies built firmware into the userspace root (src/); move it into the
# repo-level out/.
firmware: ## Build the firmware into out/ (the default target; QMK_JOBS=1 is serial)
	qmk userspace-compile -j $(QMK_JOBS)
	mkdir -p out
	mv -f src/*.bin out/

# Compile database for clang-tidy. QMK builds the firmware to record the
# compile commands, and writes the database into the userspace root (src/),
# next to the firmware copy. The firmware goes into out/, as `firmware` puts
# it, so one build gives both; the link puts the database at the repo root,
# where the tools look for it.
compiledb: ## Build the firmware into out/ and write compile_commands.json
	qmk compile -j $(QMK_JOBS) --compiledb -kb zsa/moonlander/reva -km qgmlwy
	mkdir -p out
	mv -f src/*.bin out/
	ln -sf src/compile_commands.json compile_commands.json

.PHONY: firmware compiledb

##@ Static analysis

# This userspace's C sources, the only files analysed
C_SOURCES := $(shell find src/keyboards -name '*.c' | LC_ALL=C sort)

# Every check that `analyze` runs
ANALYZE_CHECKS := clang-tidy

# All sources in one run. keymap.c has no compile database entry: QMK compiles
# it only inside quantum/keymap_introspection.c, after
# community_modules_introspection.h (which declares the modules' API, such as
# oryx.h). clang-tidy infers its command from a QMK entry, and the include
# gives it that same prelude. --extra-arg, unlike the ExtraArgs config option,
# lands before the inferred command's `--`, so after QMK's own -include list.
clang-tidy: ## Run clang-tidy only (needs the compile database)
	@time clang-tidy --quiet -p . \
		--extra-arg=-include --extra-arg=community_modules_introspection.h \
		$(C_SOURCES)

# Documentation check, configured by the repo-level Doxyfile, whose paths are
# relative to the repo root
ANALYZE_CHECKS += doxygen

doxygen: ## Run the Doxygen documentation check only
	@time doxygen Doxyfile

# Runs every check, then fails if any did: with --keep-going a failing check
# never stops the others, and make exits non-zero at the end. Only this inner
# make keeps going, so the firmware build still stops at its first error. The
# total covers the checks, not the compile database build. After a
# `make compiledb`, `make --assume-old=compiledb analyze` runs the checks
# without a second build.
analyze: compiledb ## Run every static analysis check (clang-tidy, Doxygen)
	@TIMEFORMAT='timing analyze-total %Rs'; \
		time $(MAKE) --keep-going --no-print-directory $(ANALYZE_CHECKS)

.PHONY: clang-tidy doxygen analyze

##@ Standards checker

# The checker in tools/check_standards/ links Ubuntu 24.04's tree-sitter
# packages: libtree-sitter-dev and the C grammar of tree-sitter-c-src, whose
# generated parser is compiled in. Only the Lint job builds it, on the Ubuntu
# release and package versions setup.sh installs.

# Root the packages are installed under: empty for the system, or a directory
# `dpkg -x` unpacked them into
TREE_SITTER_ROOT ?=
MULTIARCH        := $(shell $(CC) -print-multiarch)
TREE_SITTER_LIB  := $(TREE_SITTER_ROOT)/usr/lib/$(MULTIARCH)/libtree-sitter.a
TREE_SITTER_INC  := -I$(TREE_SITTER_ROOT)/usr/include
# tree-sitter-c-src installs its parser under a directory named after its version
GRAMMAR_DIR      := $(TREE_SITTER_ROOT)/usr/src/tree-sitter/c
GRAMMAR          := $(firstword $(wildcard $(GRAMMAR_DIR)/*/parser/src/parser.c))

# Stops the checker build when the grammar is missing. Only the checker's
# recipes expand it, so every other target works without tree-sitter.
require_grammar = $(if $(GRAMMAR),,$(error No C grammar in $(GRAMMAR_DIR): \
	install tree-sitter-c-src))

CFLAGS   ?= -O2
WARNINGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
	-Wstrict-prototypes -Wmissing-prototypes -Werror

CHECKER_DIR   := tools/check_standards
CHECKER_BUILD := $(CHECKER_DIR)/build
CHECKER       := $(CHECKER_BUILD)/check_standards

checker: $(CHECKER) ## Build the standards checker

$(CHECKER_BUILD):
	mkdir -p $@

$(CHECKER_BUILD)/check_standards.o: $(CHECKER_DIR)/check_standards.c | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) $(WARNINGS) -c -o $@ $<

# Generated code: built as it is, without the warnings
$(CHECKER_BUILD)/grammar.o: $(GRAMMAR) | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) -w -c -o $@ $<

$(CHECKER): $(CHECKER_BUILD)/check_standards.o $(CHECKER_BUILD)/grammar.o \
	$(TREE_SITTER_LIB)
	$(CC) $(LDFLAGS) -o $@ $^

# Every directory test/<case>/ is one test. Its .c, .h and .inc files are the
# checker's arguments, in sorted order, and <case>.expected is the checker's
# output and exit status for them. The paths are relative to
# tools/check_standards/, where the checker runs, so they match the goldens.
# Each input is named <case>.c, <case>.h or <case>.inc, or, in a case whose
# inputs are checked in a set order, <case>_<role> with roles that sort in that
# order: first, second, third. An input that can't be read, as in the
# unreadable case, is a symlink to a file that doesn't exist.
CASES     := $(sort $(basename $(notdir \
	$(wildcard $(CHECKER_DIR)/test/*/*.expected))))
arguments   = $(patsubst $(CHECKER_DIR)/%,%,$(sort $(wildcard \
	$(addprefix $(CHECKER_DIR)/test/$(1)/*,.c .h .inc))))
CASE_TESTS := $(CASES:%=$(CHECKER_BUILD)/test/%)

# Each case is its own target and only this inner make keeps going, so every
# failing case shows its diff and make exits non-zero at the end
checker-test: $(CHECKER) ## Test the standards checker against its golden fixtures
	@$(MAKE) --keep-going --no-print-directory $(CASE_TESTS)
	@echo "$(words $(CASES)) fixtures match"

$(CASE_TESTS): $(CHECKER_BUILD)/test/%: $(CHECKER)
	@mkdir -p $(@D)
	@{ cd $(CHECKER_DIR) && $(CURDIR)/$(CHECKER) $(call arguments,$*); \
		echo "exit $$?"; } >$@.out
	@diff -u $(CHECKER_DIR)/test/$*/$*.expected $@.out

.PHONY: checker checker-test $(CASE_TESTS)

##@ General

clean: ## Remove out/ and the standards checker build
	rm -rf out $(CHECKER_BUILD)

# Lists every target that has a `## description`, under its `##@` group
help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-14s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
