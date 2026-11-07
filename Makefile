
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

# Doxygen makes only the last directory of OUTPUT_DIRECTORY
doxygen: ## Run the Doxygen check
	@mkdir -p .build
	@time doxygen Doxyfile

# gcc-strict start

ANALYZE_CHECKS += gcc-strict

# QMK's build cannot get these flags, because keymap.c compiles as a part of a
# QMK file
GCC_STRICT_FLAGS := -Wextra -Wconversion -Wsign-conversion -Wshadow \
	-Wmissing-prototypes -Wmissing-declarations -Wcast-qual -Wcast-align \
	-Wpointer-arith -Wwrite-strings -Wundef -Wduplicated-cond \
	-Wduplicated-branches -Wlogical-op -Wnull-dereference -Wjump-misses-init \
	-Wdouble-promotion -Wformat=2 -Wswitch-enum -Woverride-init \
	-Wredundant-decls -Wold-style-definition -Wbad-function-cast \
	-Wfloat-equal -Wvla -Walloca -fanalyzer
GCC_STRICT_DATABASE := .build/gcc-strict/compile_commands.json

gcc-strict: ## Run gcc with more warnings on the keymap (needs the compile database)
	@mkdir -p $(dir $(GCC_STRICT_DATABASE))
	@jq --arg userspace $(QMK_USERSPACE) \
		--arg flags "$(GCC_STRICT_FLAGS)" -f src/ci/gcc-strict-entries.jq \
		.build/compile_commands.json >$(GCC_STRICT_DATABASE)
	@time GCC_STRICT_DATABASE=$(GCC_STRICT_DATABASE) src/ci/run-gcc-strict.sh

.PHONY: gcc-strict

# gcc-strict end

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
# CMakeLists.txt reads the same file, so the unit tests get the same flags
WARNINGS := $(shell grep -v '^[#]' strict-c-flags.txt)

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

# Not in analyze, because the firmware CI job has no tree-sitter headers
checker-clang-tidy: checker-compiledb ## Run clang-tidy on the standards checker
	@time clang-tidy --quiet -p $(CHECKER_BUILD) $(CHECKER_SOURCES)

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

.PHONY: checker checker-compiledb checker-clang-tidy checker-test $(CASE_TESTS)

##@ Unit tests

# Unit test builds need the cmake and libcmocka-dev versions that versions.env pins.
# CMakeLists.txt and CMakePresets.json set up the build in .build/cmake/

# Empty for the system, or the directory of a `dpkg -x` extraction of both packages
UNIT_TEST_TOOLS_ROOT ?=
# GitHub runners have a newer cmake earlier on PATH, so make calls the pinned one
CMAKE := $(UNIT_TEST_TOOLS_ROOT)/usr/bin/cmake
CTEST := $(UNIT_TEST_TOOLS_ROOT)/usr/bin/ctest
# An extracted cmake needs its libraries, and finds cmocka through the prefix path
UNIT_TEST_ENVIRONMENT = $(if $(UNIT_TEST_TOOLS_ROOT), \
	LD_LIBRARY_PATH=$(UNIT_TEST_TOOLS_ROOT)/usr/lib/$(MULTIARCH) \
	CMAKE_PREFIX_PATH=$(UNIT_TEST_TOOLS_ROOT)/usr)

# More ctest arguments, for example QMK_TEST_ARGS='-R keymap_base' for some cases
QMK_TEST_ARGS ?=

# CMake asks the QMK CLI for the qmk_firmware checkout, or uses QMK_HOME if set
qmk-test-compiledb: ## Configure the unit tests and write their compile database
	$(UNIT_TEST_ENVIRONMENT) $(CMAKE) --preset unit-tests

qmk-test: qmk-test-compiledb ## Build and run the keymap unit tests
	$(UNIT_TEST_ENVIRONMENT) $(CMAKE) --build --preset unit-tests --parallel $(QMK_JOBS)
	$(UNIT_TEST_ENVIRONMENT) $(CTEST) --preset unit-tests --label-regex '^qmk$$' \
		$(QMK_TEST_ARGS)

QMK_TEST_SOURCES := $(shell find test/qmk -name '*.c' | LC_ALL=C sort)

ANALYZE_CHECKS += qmk-test-clang-tidy

# clang-tidy with no file fails, but a run with no test is not a finding
qmk-test-clang-tidy: qmk-test-compiledb ## Run clang-tidy on the keymap unit tests
	@$(if $(QMK_TEST_SOURCES),time clang-tidy --quiet -p .build/cmake \
		$(QMK_TEST_SOURCES),echo "No keymap unit test sources")

.PHONY: qmk-test-compiledb qmk-test qmk-test-clang-tidy

##@ General

clean: ## Remove .build/
	rm -rf .build

help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-18s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
