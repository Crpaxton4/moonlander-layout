# Builds the standards checker and tests it against its golden fixtures.
#
# It links Ubuntu 24.04's tree-sitter packages: libtree-sitter-dev and the C grammar
# of tree-sitter-c-src, whose generated parser is compiled in. Only the Lint job runs
# it, on the Ubuntu release and package versions setup.sh installs; the Static
# analysis job's container ships a different tree-sitter.

# Root the packages are installed under: empty for the system, or a directory
# `dpkg -x` unpacked them into
TREE_SITTER_ROOT ?=
MULTIARCH        := $(shell $(CC) -print-multiarch)
TREE_SITTER_LIB  := $(TREE_SITTER_ROOT)/usr/lib/$(MULTIARCH)/libtree-sitter.a
# tree-sitter-c-src installs its parser under a directory named after its version
GRAMMAR_DIR      := $(TREE_SITTER_ROOT)/usr/src/tree-sitter/c
GRAMMAR          := $(firstword $(wildcard $(GRAMMAR_DIR)/*/parser/src/parser.c))
ifeq ($(GRAMMAR),)
$(error No C grammar in $(GRAMMAR_DIR): install tree-sitter-c-src)
endif

CFLAGS   ?= -O2
CPPFLAGS += -I$(TREE_SITTER_ROOT)/usr/include
WARNINGS := -std=c11 -Wall -Wextra -Wpedantic -Wconversion -Wshadow \
	-Wstrict-prototypes -Wmissing-prototypes -Werror

BUILD   := build
CHECKER := $(BUILD)/check_standards

.PHONY: all test
all: $(CHECKER)

$(BUILD):
	mkdir -p $@

$(BUILD)/check_standards.o: check_standards.c | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(WARNINGS) -c -o $@ $<

# Generated code: built as it is, without the warnings
$(BUILD)/grammar.o: $(GRAMMAR) | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -w -c -o $@ $<

$(CHECKER): $(BUILD)/check_standards.o $(BUILD)/grammar.o $(TREE_SITTER_LIB)
	$(CC) $(LDFLAGS) -o $@ $^

# Every fixtures/<case>.expected is one test: the checker's output and exit status
# for the files of the case. A case's files are fixtures/<case>.c, .h and .inc, or
# ARGUMENTS_<case> when that is set.
ARGUMENTS_multi_file := fixtures/multi_file_first.c fixtures/multi_file_second.c
ARGUMENTS_unreadable := fixtures/unreadable_before.c fixtures/no_such_file.c \
	fixtures/unreadable_after.h
CASES     := $(patsubst fixtures/%.expected,%,$(sort $(wildcard fixtures/*.expected)))
arguments  = $(or $(ARGUMENTS_$(1)),$(wildcard $(addprefix fixtures/$(1),.c .h .inc)))

# Each case is its own target and --keep-going runs them all, so every failing case
# shows its diff
MAKEFLAGS += --keep-going

test: $(CASES:%=$(BUILD)/test/%)
	@echo "$(words $(CASES)) fixtures match"

.PHONY: $(CASES:%=$(BUILD)/test/%)
$(CASES:%=$(BUILD)/test/%): $(BUILD)/test/%: $(CHECKER)
	@mkdir -p $(BUILD)/test
	@{ ./$(CHECKER) $(call arguments,$*); echo "exit $$?"; } >$@.out
	@diff -u fixtures/$*.expected $@.out
