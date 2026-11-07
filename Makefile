
# Bash `time` prints TIMEFORMAT also if the timed command fails
SHELL := /bin/bash
export TIMEFORMAT = timing $@ %Rs

.DEFAULT_GOAL := firmware

##@ Firmware

# From the repository root, only QMK_USERSPACE points QMK to this src/qmk/
export QMK_USERSPACE := $(CURDIR)/src/qmk

# QMK runs one job if nothing sets the number of jobs
QMK_JOBS ?= $(shell nproc)

FIRMWARE := zsa_moonlander_reva_qgmlwy

# CI sets the suffix for PR and release builds. Outside a git work tree, the
# suffix is empty
FIRMWARE_SUFFIX ?= $(shell git rev-parse --verify --quiet --short=7 HEAD 2>/dev/null \
	| sed 's/^/-/')

# With changes in src/qmk/ that have no commit, the commit does not identify the
# firmware, so the name also gets the start of its sha256
define name_firmware
	@set -e; name=$(FIRMWARE)$(FIRMWARE_SUFFIX); \
	if [ -n "$$(git status --porcelain -- src/qmk 2>/dev/null)" ]; then \
		hash=$$(sha256sum .build/$(FIRMWARE).bin); \
		name=$$name-dirty-$${hash:0:8}; \
	fi; \
	if [ "$$name" != $(FIRMWARE) ]; then \
		mv -f .build/$(FIRMWARE).bin .build/$$name.bin; \
	fi; \
	echo "firmware .build/$$name.bin"
endef

# QMK copies the firmware into src/qmk/, but all build output goes into .build/
firmware: ## Build the firmware (default target)
	qmk userspace-compile -j $(QMK_JOBS)
	mkdir -p .build
	mv -f src/qmk/*.bin .build/
	$(name_firmware)

KEYMAP_C := $(CURDIR)/src/qmk/keyboards/zsa/moonlander/keymaps/qgmlwy/keymap.c

# QMK has no option for the database location. The new location keeps all paths
# valid, because the entries contain absolute directories
compiledb: checker-compiledb ## Build the firmware and the compile databases
	qmk compile -j $(QMK_JOBS) --compiledb -kb zsa/moonlander/reva -km qgmlwy
	mkdir -p .build
	mv -f src/qmk/*.bin .build/
	jq --arg keymap $(KEYMAP_C) -f src/ci/add-keymap-compile-entry.jq \
		src/qmk/compile_commands.json >.build/compile_commands.json
	rm -f src/qmk/compile_commands.json
	$(name_firmware)

.PHONY: firmware compiledb

##@ Static analysis

C_SOURCES := $(shell find src/qmk/keyboards -name '*.c' | LC_ALL=C sort)

ANALYZE_CHECKS := clang-tidy

clang-tidy: ## Run clang-tidy (needs the compile database)
	@time clang-tidy --quiet -p .build $(C_SOURCES)

ANALYZE_CHECKS += doxygen

doxygen: ## Run the Doxygen check
	@time doxygen Doxyfile

# Only this inner make continues after a failed check, so the firmware build
# still stops at its first error
analyze: compiledb ## Run all static analysis checks
	@TIMEFORMAT='timing analyze-total %Rs'; \
		time $(MAKE) --keep-going --no-print-directory $(ANALYZE_CHECKS)

.PHONY: clang-tidy doxygen analyze

##@ Standards checker

# Checker builds need the tree-sitter package versions that versions.env pins

# Empty for the system, or the directory of a `dpkg -x` extraction
TREE_SITTER_ROOT ?=
MULTIARCH        := $(shell $(CC) -print-multiarch)
TREE_SITTER_LIB  := $(TREE_SITTER_ROOT)/usr/lib/$(MULTIARCH)/libtree-sitter.a
TREE_SITTER_INC  := -I$(TREE_SITTER_ROOT)/usr/include
# tree-sitter-c-src puts its parser in a directory with its version as name
GRAMMAR_DIR      := $(TREE_SITTER_ROOT)/usr/src/tree-sitter/c
GRAMMAR          := $(firstword $(wildcard $(GRAMMAR_DIR)/*/parser/src/parser.c))

# Only the checker recipes expand this, so other targets work without
# tree-sitter
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

# -MMD -MP makes a changed header rebuild the objects that include it
$(CHECKER_OBJECTS): $(CHECKER_BUILD)/%.o: $(CHECKER_DIR)/%.c | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) $(WARNINGS) -MMD -MP -c -o $@ $<

-include $(CHECKER_OBJECTS:.o=.d)

comma := ,
empty :=
space := $(empty) $(empty)
checker_object = $(1:$(CHECKER_DIR)/%.c=$(CHECKER_BUILD)/%.o)
checker_entry = {"directory": "$(CURDIR)", "file": "$(1)", "command": "$(CC) \
	$(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) $(WARNINGS) -c \
	-o $(call checker_object,$(1)) $(1)"}
checker_entries = $(foreach source,$(CHECKER_SOURCES),$(call checker_entry,$(source)))
checker_database = [$(subst }$(space){,}$(comma){,$(checker_entries))]

# No tool records the checker build, so make writes its database from the same
# flags as the object rule above. jq fails on a malformed result
checker-compiledb: | $(CHECKER_BUILD) ## Write the checker compile database
	$(file >$(CHECKER_BUILD)/compile_commands.json,$(checker_database))
	jq -e 'length > 0' $(CHECKER_BUILD)/compile_commands.json >/dev/null

# Generated code, so the build ignores its warnings
$(CHECKER_BUILD)/grammar.o: $(GRAMMAR) | $(CHECKER_BUILD)
	$(require_grammar)
	$(CC) $(CPPFLAGS) $(TREE_SITTER_INC) $(CFLAGS) -w -c -o $@ $<

$(CHECKER): $(CHECKER_OBJECTS) $(CHECKER_BUILD)/grammar.o $(TREE_SITTER_LIB)
	$(CC) $(LDFLAGS) -o $@ $^

# Arguments are the .c, .h and .inc files of a case in sort order, and
# <case>.expected is the output. Names end in _first, _second or _third to set
# an order
CASES     := $(sort $(basename $(notdir \
	$(wildcard $(CHECKER_TESTS)/*/*.expected))))
arguments   = $(sort $(wildcard $(addprefix $(CHECKER_TESTS)/$(1)/*,.c .h .inc)))
CASE_TESTS := $(CASES:%=$(CHECKER_BUILD)/test/%)

# Inner make continues after a failed case, so each failed case shows its diff
checker-test: $(CHECKER) ## Test the standards checker
	@$(MAKE) --keep-going --no-print-directory $(CASE_TESTS)
	@echo "$(words $(CASES)) fixtures match"

$(CASE_TESTS): $(CHECKER_BUILD)/test/%: $(CHECKER)
	@mkdir -p $(@D)
	@{ $(CHECKER) $(call arguments,$*); echo "exit $$?"; } >$@.out
	@diff -u $(CHECKER_TESTS)/$*/$*.expected $@.out

.PHONY: checker checker-compiledb checker-test $(CASE_TESTS)

##@ General

clean: ## Remove .build/
	rm -rf .build

help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-18s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
