/** @file
 * Precision hold of the nav mode: while thumb key L1 is held, the mouse cursor moves
 * one count per report.
 *
 * QMK's combined mouse key mode gives QK_MOUSE_ACCELERATION_0 this function, but in
 * that mode a wheel key held alone does not accelerate. Thus the keymap uses the
 * default mode, and the hold lowers QMK's top cursor speed to one count instead.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

// #endregion

// #region Functions

/**
 * Sets the top cursor speed to one count while PRECISION_HOLD_KEY is held, and sets
 * the old top speed again at the release of the hold. A tap goes on to QMK, which
 * sends Space.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event, with its tap count.
 * @return False for a hold of PRECISION_HOLD_KEY, which this function fully handles;
 *         true for other events.
 */
bool process_precision_key(uint16_t keycode, keyrecord_t *record);

// #endregion
