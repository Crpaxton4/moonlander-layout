/** @file
 * Hold keys of the qgmlwy keymap: a tap sends the tap key, a hold sends another
 * key. The bottom letter row holds send symbols, and the top letter row holds send
 * Esc and the F-keys.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

#include "layers/layers.h"

// #endregion

// #region Macros

/**
 * Symbol hold key with a tap key.
 *
 * QMK has no tap-hold keycode with a key as its hold action. Thus the key is a
 * layer-tap key of the base layer, and process_symbol_holds() sends the symbol of a
 * hold, so the layer action never occurs.
 */
#define SYMBOL_HOLD(tap_key) LT(LAYER_BASE, (tap_key))

/**
 * Hold key with a tap key, whose hold sends Esc or an F-key. It has the same form
 * as a symbol hold key, so one table gives the held key of each hold key.
 */
#define KEY_HOLD(tap_key) SYMBOL_HOLD(tap_key)

// #endregion

// #region Functions

/**
 * Gives the key that a hold of a hold key pushes.
 *
 * @param keycode Keycode of the key.
 * @return Symbol, Esc or F-key keycode, or KC_NO for a key that is not a hold key.
 */
uint16_t symbol_of_hold_key(uint16_t keycode);

/**
 * Pushes the held key on the push of a held hold key and releases it on the
 * release, so the host key repeat operates while the operator holds the key. A tap
 * goes on to QMK, which sends the tap key.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event, with its tap count.
 * @return False for a hold of a hold key, which this function fully handles; true
 *         for other events.
 */
bool process_symbol_holds(uint16_t keycode, keyrecord_t *record);

// #endregion
