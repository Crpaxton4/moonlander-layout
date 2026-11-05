/** @file
 * A file being checked, and the functions of source.c that read it and place its
 * findings. Each function is documented where it is defined, in source.c.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tree_sitter/api.h>

#include "support.h"

/** A file being checked. */
typedef struct {
    /** Path of the file as given. */
    const char *path;
    /** What tree-sitter parses: the file's text inside its kind's wrapper. */
    buffer_t parsed;
    /** Offset of the file's own text in `parsed`. */
    size_t text_start;
    /** Length of the file's own text. */
    size_t text_length;
    /** Lines the wrapper adds before the file's text. */
    uint32_t wrapper_rows;
    /** Where findings go. */
    findings_t *findings;
} source_t;

int load_source(source_t *source);

finding_t *add_finding(source_t *source, const char *rule, TSPoint point);
