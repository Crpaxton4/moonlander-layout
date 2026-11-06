/** @file
 * The declaration of check_source() from rules.c. Its documentation is at its
 * definition in rules.c.
 */

#pragma once

#include <tree_sitter/api.h>

#include "source.h"

void check_source(TSParser *parser, source_t *source);
