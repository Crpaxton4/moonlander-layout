/** @file
 * The file that the checker examines, and the functions in source.c that read the
 * file and add its findings. The documentation of each function is at its
 * definition in source.c.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <tree_sitter/api.h>

#include "support.h"

/** A file that the checker examines. */
typedef struct {
    /** The path of the file, as the command line gives it. */
    const char *path;
    /** The text that tree-sitter parses: the file text in its file kind wrapper. */
    buffer_t parsed;
    /** The offset of the file text in `parsed`. */
    size_t text_start;
    /** The length of the file text. */
    size_t text_length;
    /** The number of lines that the wrapper adds before the file text. */
    uint32_t wrapper_rows;
    /** The list that gets the findings. */
    findings_t *findings;
} source_t;

int load_source(source_t *source);

finding_t *add_finding(source_t *source, const char *rule, TSPoint point);
