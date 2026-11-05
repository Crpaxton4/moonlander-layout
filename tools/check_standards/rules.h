/** @file
 * The function of rules.c that checks one file. It is documented where it is
 * defined, in rules.c.
 */

#pragma once

#include <tree_sitter/api.h>

#include "source.h"

void check_source(TSParser *parser, source_t *source);
