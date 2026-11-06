/** @file
 * The interface of source.c.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tree_sitter/api.h>

#include "support.h"

typedef struct {
    const char *path;
    /** The file text in the wrapper of its file kind, as tree-sitter parses it. */
    buffer_t    parsed;
    size_t      text_start;
    size_t      text_length;
    uint32_t    wrapper_rows;
    findings_t *findings;
} source_t;

int load_source(source_t *source);

finding_t *add_finding(source_t *source, const char *rule, TSPoint point);
