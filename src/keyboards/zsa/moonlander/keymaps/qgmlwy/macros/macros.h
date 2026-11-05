/** @file
 * Launcher and window macros of the qgmlwy keymap.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Sends the key sequence of an `ST_MACRO_*` key when it is pressed.
 *
 * `ST_MACRO_0` to `ST_MACRO_37` send Alt+Ctrl+Shift+grave, then after 100 ms
 * Alt+Ctrl+Shift with the macro's key; `ST_MACRO_18` sends its second chord
 * without Shift. `ST_MACRO_38` to `ST_MACRO_41` send GUI+F1 to F4, then after
 * 250 ms Ctrl with the same F key.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event; only presses send.
 * @return Always true, so QMK goes on processing the key.
 */
bool process_macros(uint16_t keycode, keyrecord_t *record);
