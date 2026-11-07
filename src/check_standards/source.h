/** @file
 * Interface of source.c
 */

#pragma once

// #region Includes

#include <stddef.h>
#include <stdint.h>
#include <tree_sitter/api.h>

#include "support.h"

// #endregion

// #region Types

/** File that the checker checks, with the text that tree-sitter parses */
typedef struct {
    const char *path;         /**< File as the command line gives it */
    buffer_t    parsed;       /**< File text in the wrapper of its file kind */
    size_t      text_start;   /**< Offset of the file text in parsed */
    size_t      text_length;  /**< Bytes of the file text, without the wrapper */
    uint32_t    wrapper_rows; /**< Lines of the wrapper before the file text */
    findings_t *findings;     /**< Findings of all files of the run */
} source_t;

// #endregion

// #region Functions

/**
 * Reads the file at source->path into source->parsed.
 *
 * @param source Source with a path and an empty parsed buffer.
 * @return 0, or the errno value of the error that stopped the read.
 */
int load_source(source_t *source);

/**
 * Adds a finding at a position of the parsed text. The caller writes the message
 * of the finding with describe().
 *
 * @param source Source that contains the position.
 * @param rule   Rule name that the output shows.
 * @param point  Position in the parsed text, from 0.
 * @return New finding, which stays valid until the next add_finding() call.
 */
finding_t *add_finding(source_t *source, const char *rule, TSPoint point);

// #endregion
