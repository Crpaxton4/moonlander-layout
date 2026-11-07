/** @file
 * Interface of rules.c
 */

#pragma once

#include <tree_sitter/api.h>

#include "source.h"

/**
 * Reads, parses and checks a file, and adds its findings. A read error gives an
 * io-error finding.
 *
 * @param parser Parser with the C grammar.
 * @param source Source with a path and an empty parsed buffer.
 */
void check_source(TSParser *parser, source_t *source);
