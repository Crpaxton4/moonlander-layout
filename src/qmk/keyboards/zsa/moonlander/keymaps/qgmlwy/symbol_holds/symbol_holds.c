/** @file
 * Symbols of the symbol hold keys on the bottom letter row.
 *
 * Opening and closing symbols are at mirrored positions of the two hands. Shifted
 * symbols are 16-bit keycodes, so the code uses register_code16().
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "symbol_holds/symbol_holds.h"

// #endregion

// #region Types

/** Tap key and hold symbol of one symbol hold key */
typedef struct {
    uint16_t tap_key; /**< Key of a tap, which also identifies the key */
    uint16_t symbol;  /**< Symbol of a hold, for a US English host layout */
} symbol_hold_t;

// #endregion

// #region Data

/** Symbol hold keys from the left pinky to the right pinky */
static const symbol_hold_t symbol_holds[] = {
    {.tap_key = KC_Z, .symbol = KC_LEFT_ANGLE_BRACKET},
    {.tap_key = KC_X, .symbol = KC_LEFT_BRACKET},
    {.tap_key = KC_C, .symbol = KC_LEFT_CURLY_BRACE},
    {.tap_key = KC_V, .symbol = KC_LEFT_PAREN},
    {.tap_key = KC_J, .symbol = KC_UNDERSCORE},
    {.tap_key = KC_K, .symbol = KC_MINUS},
    {.tap_key = KC_P, .symbol = KC_RIGHT_PAREN},
    {.tap_key = KC_COMMA, .symbol = KC_RIGHT_CURLY_BRACE},
    {.tap_key = KC_DOT, .symbol = KC_RIGHT_BRACKET},
    {.tap_key = KC_SLASH, .symbol = KC_RIGHT_ANGLE_BRACKET},
};

// #endregion

// #region Public functions

uint16_t symbol_of_hold_key(uint16_t keycode) {
    for (size_t index = 0; index < ARRAY_SIZE(symbol_holds); index++) {
        if (SYMBOL_HOLD(symbol_holds[index].tap_key) == keycode) {
            return symbol_holds[index].symbol;
        }
    }
    return KC_NO;
}

bool process_symbol_holds(uint16_t keycode, keyrecord_t *record) {
    uint16_t symbol = symbol_of_hold_key(keycode);
    if (symbol == KC_NO || record->tap.count > 0) {
        return true;
    }
    if (record->event.pressed) {
        register_code16(symbol);
    } else {
        unregister_code16(symbol);
    }
    return false;
}

// #endregion
