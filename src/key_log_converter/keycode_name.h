/** @file
 * Names of the QMK keycodes of the input records
 */

#pragma once

// #region Includes

#include <stdint.h>

// #endregion

// #region Macros

/** Bytes of the longest keycode name, with its terminator */
#define KEYCODE_NAME_SIZE 48

// #endregion

// #region Functions

/**
 * Writes the name of a QMK keycode, for example `T / Left GUI` for a mod-tap key.
 * A keycode with no name gets its hexadecimal value.
 *
 * @param keycode QMK keycode after layer resolution.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
void write_keycode_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);

// #endregion
