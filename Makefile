# Run each command from the repository root as `make <target>`. `make help` lists
# the targets.

# Recipes run in bash because of its `time` keyword, which prints TIMEFORMAT also
# if the timed command fails. $@ expands for each target, so each timed recipe
# prints `timing <target> <seconds>s`.
SHELL := /bin/bash
export TIMEFORMAT = timing $@ %Rs

.DEFAULT_GOAL := firmware

##@ Firmware

# The QMK userspace is src/qmk/. QMK looks for qmk.json in the current directory
# and its parents, then in QMK_USERSPACE, then in user.overlay_dir. Thus from the
# repository root, only QMK_USERSPACE points QMK to src/qmk/ of this checkout.
export QMK_USERSPACE := $(CURDIR)/src/qmk

# The number of make jobs in the QMK firmware build. The default is one job for
# each CPU, because QMK itself runs one job if nothing sets the number.
# `make QMK_JOBS=1` builds serially. Only the QMK make runs in parallel, never the
# targets of this make.
QMK_JOBS ?= $(shell nproc)

# QMK copies the built firmware into the userspace root (src/qmk/). The recipe
# moves it into .build/ at the repository root.
firmware: ## Build the firmware into .build/ (the default target). QMK_JOBS=1 is serial.
	qmk userspace-compile -j $(QMK_JOBS)
	mkdir -p .build
	mv -f src/qmk/*.bin .build/

# The compile database for clang-tidy. QMK builds the firmware to record the
# compile commands. It writes the database into the userspace root (src/qmk/),
# next to the firmware copy. The recipe moves both into .build/, so one build
# gives both. QMK has no option for the location of the database. Each entry
# contains an absolute directory, and its file is relative to that directory or
# absolute. Thus the move keeps all paths valid.
compiledb: ## Build the firmware and the compile database into .build/
	qmk compile -j $(QMK_JOBS) --compiledb -kb zsa/moonlander/reva -km qgmlwy
	mkdir -p .build
	mv -f src/qmk/*.bin src/qmk/compile_commands.json .build/

.PHONY: firmware compiledb

##@ Static analysis

# The C sources of this userspace. The checks analyze only these files.
C_SOURCES := $(shell find src/qmk/keyboards -name '*.c' | LC_ALL=C sort)

ANALYZE_CHECKS := clang-tidy

# One run for all sources. keymap.c has no compile database entry. QMK compiles
# it only in quantum/keymap_introspection.c, after
# community_modules_introspection.h, which declares the API of the modules (for
# example oryx.h). clang-tidy infers the keymap.c command from a QMK entry, and
# the include gives it the same prelude. Unlike the ExtraArgs config option,
# --extra-arg goes before the `--` of the inferred command. Thus it comes after
# the -include list of QMK.
clang-tidy: ## Run only clang-tidy. It needs the compile database.
	@time clang-tidy --quiet -p .build \
		--extra-arg=-include --extra-arg=community_modules_introspection.h \
		$(C_SOURCES)

# The documentation check. The Doxyfile at the repository root configures it.
# The paths in the Doxyfile are relative to the repository root.
ANALYZE_CHECKS += doxygen

doxygen: ## Run only the Doxygen documentation check
	@time doxygen Doxyfile

# This target runs all checks, then fails if a check failed. With --keep-going, a
# failed check never stops the other checks, and make exits with a non-zero
# status at the end. Only this inner make continues after an error, so the
# firmware build still stops at its first error. The total time is for the
# checks, not for the compile database build. After `make compiledb`,
# `make --assume-old=compiledb analyze` runs the checks without a second build.
analyze: compiledb ## Run all static analysis checks (clang-tidy, Doxygen)
	@TIMEFORMAT='timing analyze-total %Rs'; \
		time $(MAKE) --keep-going --no-print-directory $(ANALYZE_CHECKS)

.PHONY: clang-tidy doxygen analyze

##@ Standards checker

# The checker in src/check_standards/ links the tree-sitter packages of Ubuntu
# 24.04: libtree-sitter-dev and the C grammar of tree-sitter-c-src. The build
# compiles the generated parser of the grammar into the checker. In CI, only the
# Lint job builds the checker, on the Ubuntu release and package versions that
# setup.sh installs.

# The root of the packages: empty for the system, or the directory into which
# `dpkg -x` extracted them.
TREE_SITTER_ROOT ?=
MULTIARCH        := $(shell $(CC) -print-multiarch)
TREE_SITTER_LIB  := $(TREE_SITTER_ROOT)/usr/lib/$(MULTIARCH)/libtree-sitter.a
TREE_SITTER_INC  := -I$(TREE_SITTER_ROOT)/usr/include
# tree-sitter-c-src installs its parser in a directory with its version as name.
GRAMMAR_DIR      := $(TREE_SITTER_ROOT)/usr/src/tree-sitter/c
GRAMMAR          := $(firstword $(wildcard $(GRAMMAR_DIR)/*/parser/src/parser.c))

# This stops the checker build if the grammar is missing. Only the checker
# recipes expand it, so all other targets work without tree-sitter.
require_grammar = $(if $(GRAMMAR),,$(error No C grammar in $(GRAMMAR_DIR): \
	install tree-sitter-c-src))

CFLAGS   ?= -O2
WARNINGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
	-Wstrict-prototypes -Wmissing-prototypes -Werror

CHECKER_DIR     := src/check_standards
CHECKER_TESTS   := test/check_standards
CHECKER_BUILD   := .build/check_standards
CHECKER         := $(CHECKER_BUILD)/check_standards
CHECKER_SOURCES := $(sort $(wildcard $(CHECKER_DIR)/*.c))
CHECKER_OBJECTS := $(CHECKER_SOURCES:$(CHECKER_DIR)/%.c=$(CHECKER_BUILD)/%.o)

checker: $(CHECKER) ## Build the standards checker

$(CHECKER_BUILD):
	mkdir -p $@

# Each .c file of the checker compiles to its own object. -MMD -MP writes the
# header dependencies of the object to its .d file, which the next build reads.
# Thus a changed header rebuilds the objects that include it.
$(CHECKER_OBJECTS): $(CHECKER_BUILD)/%.o: $(CHECKER_DIR)/%.c | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) $(WARNINGS) -MMD -MP -c -o $@ $<

-include $(CHECKER_OBJECTS:.o=.d)

# This is generated code, so the build compiles it as it is, without warnings.
$(CHECKER_BUILD)/grammar.o: $(GRAMMAR) | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) -w -c -o $@ $<

$(CHECKER): $(CHECKER_OBJECTS) $(CHECKER_BUILD)/grammar.o $(TREE_SITTER_LIB)
	$(CC) $(LDFLAGS) -o $@ $^

# Each directory test/check_standards/<case>/ is one test. Its .c, .h and .inc
# files are the checker arguments, in sort order. <case>.expected is the checker
# output and exit status for them. The checker runs from the repository root, so
# the paths in its output agree with the goldens.
# The name of each input is <case>.c, <case>.h or <case>.inc. In a case that
# checks its inputs in a set order, the name is <case>_<role>, with roles that
# sort in that order: first, second, third. An input that the checker cannot
# read, as in the unreadable case, is a symlink to a file that does not exist.
CASES     := $(sort $(basename $(notdir \
	$(wildcard $(CHECKER_TESTS)/*/*.expected))))
arguments   = $(sort $(wildcard $(addprefix $(CHECKER_TESTS)/$(1)/*,.c .h .inc)))
CASE_TESTS := $(CASES:%=$(CHECKER_BUILD)/test/%)

# Each case is its own target, and only this inner make continues after an error.
# Thus each failed case shows its diff, and make exits with a non-zero status at
# the end.
checker-test: $(CHECKER) ## Test the standards checker with its golden fixtures
	@$(MAKE) --keep-going --no-print-directory $(CASE_TESTS)
	@echo "$(words $(CASES)) fixtures match"

$(CASE_TESTS): $(CHECKER_BUILD)/test/%: $(CHECKER)
	@mkdir -p $(@D)
	@{ $(CHECKER) $(call arguments,$*); echo "exit $$?"; } >$@.out
	@diff -u $(CHECKER_TESTS)/$*/$*.expected $@.out

.PHONY: checker checker-test $(CASE_TESTS)

##@ General

clean: ## Remove .build/, which contains all build output
	rm -rf .build

# This target lists each target that has a `## description`, under its `##@`
# group.
help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-14s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
