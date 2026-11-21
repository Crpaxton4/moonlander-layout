/** @file
 * Grid positions and thumb key names of the Moonlander matrix positions
 */

// #region Includes

#include <stdio.h>

#include "counters.h"
#include "grid_position.h"

// #endregion

// #region Macros

/** Matrix rows of one keyboard half; the right half follows the left half */
#define HALF_ROWS 6U

/** Row of the thumb keys in the matrix rows of one half */
#define THUMB_ROW 5U

/** Grid columns of one keyboard half; the right half follows the left half */
#define HALF_GRID_COLUMNS 7U

/** Keyboard halves */
#define HALF_COUNT 2

// #endregion

// #region Data

/**
 * Thumb key names, indexed by half and matrix column, in the order of the
 * `thumb_key` enumeration of the keymap. NULL is a column with no thumb key.
 */
static const char *const thumb_names[HALF_COUNT][MATRIX_COLS] = {
    {"L1", "L2", "L3", "L0", NULL, NULL, NULL},
    {NULL, NULL, NULL, "R0", "R3", "R2", "R1"},
};

// #endregion

// #region Static functions

/**
 * Writes a position that has no grid key and no thumb key.
 *
 * @param position Matrix position.
 * @param text     Buffer of GRID_POSITION_SIZE bytes that gets the text.
 */
static void write_matrix_position(position_t position, char text[GRID_POSITION_SIZE]) {
    unsigned row = position.row;
    unsigned col = position.col;
    snprintf(text, GRID_POSITION_SIZE, "matrix %u,%u", row, col);
}

/**
 * Writes the name of a position in a thumb row.
 *
 * @param position Matrix position in a thumb row of the matrix.
 * @param text     Buffer of GRID_POSITION_SIZE bytes that gets the text.
 */
static void write_thumb_position(position_t position, char text[GRID_POSITION_SIZE]) {
    const char *thumb_name = thumb_names[position.row / HALF_ROWS][position.col];
    if (thumb_name == NULL) {
        write_matrix_position(position, text);
        return;
    }
    snprintf(text, GRID_POSITION_SIZE, "%s", thumb_name);
}

// #endregion

// #region Public functions

void write_grid_position(position_t position, char text[GRID_POSITION_SIZE]) {
    unsigned row = position.row % HALF_ROWS;
    unsigned col = position.col + position.row / HALF_ROWS * HALF_GRID_COLUMNS;
    if (position.row >= MATRIX_ROWS || position.col >= MATRIX_COLS) {
        write_matrix_position(position, text);
    } else if (row == THUMB_ROW) {
        write_thumb_position(position, text);
    } else {
        snprintf(text, GRID_POSITION_SIZE, "(%u,%u)", row, col);
    }
}

// #endregion
