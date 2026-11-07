/** @file
 * Special mode of the qgmlwy keymap: LAYER_SPECIAL with digits and symbols, on
 * until the operator ends it.
 *
 * Mode is a layer that stays on, not a momentary layer, so the operator can type a
 * full number or expression with no held key.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Starts or ends the special mode for the mode key, and ends the mode when the
 * operator pushes the same whitespace key two times in a row.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event.
 * @return False for the mode key and for a push that ends the mode, which send no
 *         key; true otherwise.
 */
bool process_special_mode(uint16_t keycode, keyrecord_t *record);
