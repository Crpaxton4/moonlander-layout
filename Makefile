
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
# valid, because the entries contain absolute directories. The CMake configure
# step writes the compile databases of the host programs and the unit tests
compiledb: ## Build the firmware and its compile database
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

ANALYZE_CHECKS += qmk-test-clang-tidy

QMK_TEST_SOURCES := $(shell find test/qmk -name '*.c' | LC_ALL=C sort)

# CMake writes the compile database of the unit tests at the configure step. CMake
# asks the QMK CLI for the qmk_firmware checkout, or uses QMK_HOME if set.
# clang-tidy with no file fails, but a run with no test is not a finding
qmk-test-clang-tidy: ## Run clang-tidy on the keymap unit tests
	cmake --preset unit-tests
	@$(if $(QMK_TEST_SOURCES),time clang-tidy --quiet -p .build/unit-tests \
		$(QMK_TEST_SOURCES),echo "No keymap unit test sources")

.PHONY: qmk-test-clang-tidy

# Only this inner make continues after a failed check, so the firmware build
# still stops at its first error
analyze: compiledb ## Run all static analysis checks
	@TIMEFORMAT='timing analyze-total %Rs'; \
		time $(MAKE) --keep-going --no-print-directory $(ANALYZE_CHECKS)

.PHONY: clang-tidy doxygen analyze

##@ General

clean: ## Remove .build/
	rm -rf .build

help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-22s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
