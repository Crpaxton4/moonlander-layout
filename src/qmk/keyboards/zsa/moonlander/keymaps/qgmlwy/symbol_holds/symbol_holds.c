/** @file
 * Held keys of the hold keys on the top and bottom letter rows.
 *
 * Opening and closing symbols are at mirrored positions of the two hands. Shifted
 * symbols are 16-bit keycodes, so the code uses register_code16().
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "symbol_holds/symbol_holds.h"

// #endregion

// #region Types

/** Tap key and held key of one hold key */
typedef struct {
    uint16_t tap_key;  /**< Key of a tap, which also identifies the key */
    uint16_t held_key; /**< Key of a hold, for a US English host layout */
} hold_key_t;

// #endregion

// #region Data

/**
 * Hold keys from the left pinky to the right pinky, top letter row first. The case
 * mode keys at (1,6) and (1,7) have their F-key as their tap key, because a
 * layer-tap key cannot hold a custom keycode. Case modes take each tap of them.
 */
static const hold_key_t hold_keys[] = {
    {.tap_key = KC_TAB, .held_key = KC_ESCAPE},
    {.tap_key = KC_Q, .held_key = KC_F1},
    {.tap_key = KC_G, .held_key = KC_F2},
    {.tap_key = KC_M, .held_key = KC_F3},
    {.tap_key = KC_L, .held_key = KC_F4},
    {.tap_key = KC_W, .held_key = KC_F5},
    {.tap_key = KC_F6, .held_key = KC_F6},
    {.tap_key = KC_F7, .held_key = KC_F7},
    {.tap_key = KC_Y, .held_key = KC_F8},
    {.tap_key = KC_F, .held_key = KC_F9},
    {.tap_key = KC_U, .held_key = KC_F10},
    {.tap_key = KC_B, .held_key = KC_F11},
    {.tap_key = KC_SEMICOLON, .held_key = KC_F12},
    {.tap_key = KC_Z, .held_key = KC_LEFT_ANGLE_BRACKET},
    {.tap_key = KC_X, .held_key = KC_LEFT_BRACKET},
    {.tap_key = KC_C, .held_key = KC_LEFT_CURLY_BRACE},
    {.tap_key = KC_V, .held_key = KC_LEFT_PAREN},
    {.tap_key = KC_J, .held_key = KC_UNDERSCORE},
    {.tap_key = KC_K, .held_key = KC_MINUS},
    {.tap_key = KC_P, .held_key = KC_RIGHT_PAREN},
    {.tap_key = KC_COMMA, .held_key = KC_RIGHT_CURLY_BRACE},
    {.tap_key = KC_DOT, .held_key = KC_RIGHT_BRACKET},
    {.tap_key = KC_SLASH, .held_key = KC_RIGHT_ANGLE_BRACKET},
};

// #endregion

// #region Public functions

uint16_t symbol_of_hold_key(uint16_t keycode) {
    for (size_t index = 0; index < ARRAY_SIZE(hold_keys); index++) {
        if (SYMBOL_HOLD(hold_keys[index].tap_key) == keycode) {
            return hold_keys[index].held_key;
        }
    }
    return KC_NO;
}

bool process_symbol_holds(uint16_t keycode, keyrecord_t *record) {
    uint16_t held_key = symbol_of_hold_key(keycode);
    if (held_key == KC_NO || record->tap.count > 0) {
        return true;
    }
    if (record->event.pressed) {
        register_code16(held_key);
    } else {
        unregister_code16(held_key);
    }
    return false;
}

// #endregion
