/** @file
 * Key positions as the keymap names them: grid row and column, or thumb key name
 */

#pragma once

// #region Includes

#include "record.h"

// #endregion

// #region Macros

/** Bytes of the longest position text, with its terminator */
#define GRID_POSITION_SIZE 16

/** Bytes of the longest position field, with two quotation marks */
#define POSITION_FIELD_SIZE (GRID_POSITION_SIZE + 2)

// #endregion

// #region Functions

/**
 * Writes a matrix position as the keymap names it: `(2,3)` for grid row 2 and grid
 * column 3, or `L0` to `L3` and `R0` to `R3` for a thumb key. A position with no
 * name, for example a position outside the matrix, gets `matrix`, the matrix row
 * and the matrix column.
 *
 * @param position Matrix position.
 * @param text     Buffer of GRID_POSITION_SIZE bytes that gets the text.
 */
void write_grid_position(position_t position, char text[GRID_POSITION_SIZE]);

/**
 * Writes a matrix position as one field of a CSV line. A text with a comma gets
 * quotation marks.
 *
 * @param position Matrix position.
 * @param field    Buffer of POSITION_FIELD_SIZE bytes that gets the field.
 */
void write_position_field(position_t position, char field[POSITION_FIELD_SIZE]);

// #endregion
