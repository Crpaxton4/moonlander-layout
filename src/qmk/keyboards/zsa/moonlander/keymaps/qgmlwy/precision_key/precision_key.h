/** @file
 * Precision key of the nav mode, thumb key R1: while it is held, the mouse cursor
 * moves one count per report.
 *
 * QMK's combined mouse key mode gives QK_MOUSE_ACCELERATION_0 this function, but in
 * that mode a wheel key held alone does not accelerate. Thus the keymap uses the
 * default mode, and this key lowers QMK's top cursor speed to one count instead.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

// #endregion

// #region Functions

/**
 * Sets the top cursor speed to one count while QK_MOUSE_ACCELERATION_0 is held, and
 * sets the old top speed again at its release. QMK's own speed steps for that key
 * then have no effect.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False for QK_MOUSE_ACCELERATION_0, true for all other keys.
 */
bool process_precision_key(uint16_t keycode, keyrecord_t *record);

// #endregion
