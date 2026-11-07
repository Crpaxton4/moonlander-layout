/** @file
 * Markdown blocks in the Prettier format, so that a Prettier check of a report passes
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

/** Bytes of one table cell, with its terminator */
#define CELL_SIZE 64

/** Most columns of a table */
#define MAX_TABLE_COLUMNS 5

/** Alignment of a table column */
typedef enum {
    ALIGN_LEFT,  /**< Text */
    ALIGN_RIGHT, /**< Numbers */
} alignment_t;

/** Heading and alignment of a table column */
typedef struct {
    const char *title;     /**< Column heading */
    alignment_t alignment; /**< Alignment of the heading and the cells */
} column_t;

/** Cells of one table row. A cell has no `|` character */
typedef struct {
    char cells[MAX_TABLE_COLUMNS][CELL_SIZE]; /**< Cell texts, by column */
} table_row_t;

/** Table that collects its rows, because the column widths come from all rows */
typedef struct {
    const column_t *columns;      /**< Columns, column_count items */
    size_t          column_count; /**< Columns, MAX_TABLE_COLUMNS at most */
    table_row_t    *rows;         /**< Rows, or NULL before the first row */
    size_t          row_count;    /**< Rows in use */
    size_t          capacity;     /**< Rows that the array can hold */
} table_t;

/**
 * Writes a heading or a paragraph after a blank line.
 *
 * @param file File.
 * @param text Text of one line.
 */
void write_block(FILE *file, const char *text);

/**
 * Adds an empty row to a table.
 *
 * @param table Table.
 * @return Row in the table. The next add can move it.
 */
table_row_t *add_table_row(table_t *table);

/**
 * Writes a table after a blank line, with each column as wide as its widest cell,
 * or a paragraph for a table with no rows. Then frees the rows.
 *
 * @param file  File.
 * @param table Table.
 */
void write_table(FILE *file, table_t *table);
