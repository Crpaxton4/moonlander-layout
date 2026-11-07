
# Firmware name takes a bash substring of the sha256
SHELL := /bin/bash

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

##@ General

clean: ## Remove .build/
	rm -rf .build

help: ## List the targets
	@awk 'BEGIN { FS = ":.*## "; print "Usage: make [target]" } \
		/^##@ / { printf "\n%s\n", substr($$0, 5) } \
		/^[a-z-]+:.*## / { printf "  %-22s %s\n", $$1, $$2 }' $(MAKEFILE_LIST)

.PHONY: clean help
