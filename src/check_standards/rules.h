/** @file
 * The interface of rules.c.
 */

#pragma once

#include <tree_sitter/api.h>

#include "source.h"

void check_source(TSParser *parser, source_t *source);
