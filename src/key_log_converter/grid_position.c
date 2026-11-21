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
 * Finds the name of the thumb key at a matrix position.
 *
 * @param position Matrix position.
 * @return Name, or NULL for a position that is not a thumb key.
 */
static const char *find_thumb_name(position_t position) {
    if (position.row >= MATRIX_ROWS || position.col >= MATRIX_COLS) {
        return NULL;
    }
    if (position.row % HALF_ROWS != THUMB_ROW) {
        return NULL;
    }
    return thumb_names[position.row / HALF_ROWS][position.col];
}

/**
 * Tells if a matrix position is in a grid row.
 *
 * @param position Matrix position.
 * @return true for a position in the matrix that is not in a thumb row.
 */
static bool is_in_grid(position_t position) {
    if (position.row >= MATRIX_ROWS || position.col >= MATRIX_COLS) {
        return false;
    }
    return position.row % HALF_ROWS != THUMB_ROW;
}

// #endregion

// #region Public functions

void write_grid_position(position_t position, char text[GRID_POSITION_SIZE]) {
    const char *thumb_name = find_thumb_name(position);
    unsigned    row        = position.row;
    unsigned    col        = position.col;
    if (thumb_name != NULL) {
        snprintf(text, GRID_POSITION_SIZE, "%s", thumb_name);
    } else if (is_in_grid(position)) {
        unsigned grid_column = col + row / HALF_ROWS * HALF_GRID_COLUMNS;
        snprintf(text, GRID_POSITION_SIZE, "(%u,%u)", row % HALF_ROWS, grid_column);
    } else {
        snprintf(text, GRID_POSITION_SIZE, "matrix %u,%u", row, col);
    }
}

// #endregion
