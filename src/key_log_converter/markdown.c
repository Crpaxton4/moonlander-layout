/** @file
 * Markdown tables with padded cells, as Prettier writes them
 */

// #region Includes

#include <stdlib.h>
#include <string.h>

#include "markdown.h"
#include "support.h"

// #endregion

// #region Macros

/** Prettier makes each column at least as wide as the three dashes of a delimiter */
#define MIN_COLUMN_WIDTH 3

/** Bits that mark a UTF-8 continuation byte, which adds no width */
#define UTF8_CONTINUATION_MASK 0xC0

/** Value of a UTF-8 continuation byte under UTF8_CONTINUATION_MASK */
#define UTF8_CONTINUATION_BITS 0x80

/** Paragraph in place of a table with no rows */
#define EMPTY_TABLE_TEXT "None."

// #endregion

// #region Data

/** End of the delimiter of each alignment, after the dashes */
static const char *const delimiter_ends[] = {
    [ALIGN_LEFT]  = "",
    [ALIGN_RIGHT] = ":",
};

// #endregion

// #region Static functions

/**
 * Gives the width of a text. Each UTF-8 character has the width 1, as for Prettier.
 *
 * @param text UTF-8 text.
 * @return Characters in the text.
 */
static size_t display_width(const char *text) {
    size_t width = 0;
    for (const char *byte = text; *byte != '\0'; byte++) {
        if (((unsigned char)*byte & UTF8_CONTINUATION_MASK) != UTF8_CONTINUATION_BITS) {
            width++;
        }
    }
    return width;
}

/**
 * Gives the larger of two widths.
 *
 * @param width Width.
 * @param other Other width.
 * @return Larger width.
 */
static size_t larger_width(size_t width, size_t other) {
    return other > width ? other : width;
}

/**
 * Writes spaces.
 *
 * @param file  File.
 * @param count Number of spaces.
 */
static void write_spaces(FILE *file, size_t count) {
    for (size_t index = 0; index < count; index++) {
        fputc(' ', file);
    }
}

/**
 * Writes a text with spaces on the side that the alignment gives.
 *
 * @param file   File.
 * @param text   Cell text.
 * @param width  Column width, at least the width of the text.
 * @param column Column of the text.
 */
static void write_cell_text(FILE *file, const char *text, size_t width,
                            const column_t *column) {
    size_t padding = width - display_width(text);
    if (column->alignment == ALIGN_RIGHT) {
        write_spaces(file, padding);
    }
    fputs(text, file);
    if (column->alignment == ALIGN_LEFT) {
        write_spaces(file, padding);
    }
}

/**
 * Writes one table line.
 *
 * @param file   File.
 * @param table  Table.
 * @param widths Width of each column.
 * @param texts  Text of each column.
 */
static void write_cell_line(FILE *file, const table_t *table, const size_t *widths,
                            const char *const *texts) {
    fputc('|', file);
    for (size_t column = 0; column < table->column_count; column++) {
        fputc(' ', file);
        write_cell_text(file, texts[column], widths[column], &table->columns[column]);
        fputs(" |", file);
    }
    fputc('\n', file);
}

/**
 * Gives the width of each column: its widest cell or heading.
 *
 * @param table  Table.
 * @param widths Width of each column, MAX_TABLE_COLUMNS items.
 */
static void measure_columns(const table_t *table, size_t *widths) {
    for (size_t column = 0; column < table->column_count; column++) {
        widths[column] =
            larger_width(MIN_COLUMN_WIDTH, display_width(table->columns[column].title));
        for (size_t row = 0; row < table->row_count; row++) {
            size_t cell_width = display_width(table->rows[row].cells[column]);
            widths[column]    = larger_width(widths[column], cell_width);
        }
    }
}

/**
 * Writes the delimiter of a column: dashes and the end of its alignment.
 *
 * @param delimiter Buffer of CELL_SIZE bytes that gets the delimiter.
 * @param width     Column width, less than CELL_SIZE.
 * @param column    Column of the delimiter.
 */
static void format_delimiter(char *delimiter, size_t width, const column_t *column) {
    const char *end    = delimiter_ends[column->alignment];
    size_t      dashes = width - strlen(end);
    memset(delimiter, '-', dashes);
    snprintf(delimiter + dashes, CELL_SIZE - dashes, "%s", end);
}

/**
 * Writes the heading line and the delimiter line.
 *
 * @param file   File.
 * @param table  Table.
 * @param widths Width of each column, less than CELL_SIZE.
 */
static void write_heading_lines(FILE *file, const table_t *table,
                                const size_t *widths) {
    const char *titles[MAX_TABLE_COLUMNS];
    char        delimiters[MAX_TABLE_COLUMNS][CELL_SIZE];
    const char *delimiter_texts[MAX_TABLE_COLUMNS];
    for (size_t column = 0; column < table->column_count; column++) {
        const column_t *heading = &table->columns[column];
        titles[column]          = heading->title;
        format_delimiter(delimiters[column], widths[column], heading);
        delimiter_texts[column] = delimiters[column];
    }
    write_cell_line(file, table, widths, titles);
    write_cell_line(file, table, widths, delimiter_texts);
}

/**
 * Writes the lines of the table rows.
 *
 * @param file   File.
 * @param table  Table.
 * @param widths Width of each column.
 */
static void write_body_lines(FILE *file, const table_t *table, const size_t *widths) {
    const char *texts[MAX_TABLE_COLUMNS];
    for (size_t row = 0; row < table->row_count; row++) {
        for (size_t column = 0; column < table->column_count; column++) {
            texts[column] = table->rows[row].cells[column];
        }
        write_cell_line(file, table, widths, texts);
    }
}

// #endregion

// #region Public functions

void write_block(FILE *file, const char *text) {
    fprintf(file, "\n%s\n", text);
}

table_row_t *add_table_row(table_t *table) {
    table->rows =
        reserve(table->rows, sizeof(table_row_t), &table->capacity, table->row_count);
    table_row_t *row = &table->rows[table->row_count++];
    memset(row, 0, sizeof(*row));
    return row;
}

void write_table(FILE *file, table_t *table) {
    size_t widths[MAX_TABLE_COLUMNS];
    if (table->row_count == 0) {
        write_block(file, EMPTY_TABLE_TEXT);
        return;
    }
    measure_columns(table, widths);
    fputc('\n', file);
    write_heading_lines(file, table, widths);
    write_body_lines(file, table, widths);
    free(table->rows);
    table->rows      = NULL;
    table->row_count = 0;
    table->capacity  = 0;
}

// #endregion
