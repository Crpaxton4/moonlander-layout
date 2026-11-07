/** @file
 * Dual-function keys of the qgmlwy keymap: one key for a tap, another for a hold.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Sends the tap or hold key of a dual-function key: COPY_KEY or PASTE_KEY.
 *
 * A tap sends the tap key, a hold sends the hold key until the key is released.
 * QMK's layer-tap handling of these keycodes never runs.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event, with its tap count.
 * @return False for a dual-function key, which this function fully handles; true
 *         for any other key.
 */
bool process_dual_function(uint16_t keycode, keyrecord_t *record);
