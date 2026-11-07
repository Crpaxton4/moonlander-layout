/** @file
 * Symbol hold keys of the qgmlwy keymap: a tap sends the key, a hold sends a
 * symbol.
 */

#pragma once

#include QMK_KEYBOARD_H

#include "layers/layers.h"

/**
 * Symbol hold key with a tap key.
 *
 * QMK has no tap-hold keycode with a key as its hold action. Thus the key is a
 * layer-tap key of the base layer, and process_symbol_holds() sends the symbol of a
 * hold, so the layer action never occurs.
 */
#define SYMBOL_HOLD(tap_key) LT(LAYER_BASE, (tap_key))

/**
 * Gives the symbol that a hold of a symbol hold key pushes.
 *
 * @param keycode Keycode of the key.
 * @return Symbol keycode, or KC_NO for a key that is not a symbol hold key.
 */
uint16_t symbol_of_hold_key(uint16_t keycode);

/**
 * Pushes the symbol on the push of a held symbol hold key and releases it on the
 * release. A tap goes on to QMK, which sends the tap key.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event, with its tap count.
 * @return False for a hold of a symbol hold key, which this function fully
 *         handles; true for other events.
 */
bool process_symbol_holds(uint16_t keycode, keyrecord_t *record);
