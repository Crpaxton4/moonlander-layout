/** @file
 * Keycodes of the qgmlwy keymap that QMK does not have.
 *
 * Layers and the key event hooks both use these keycodes, so one header holds their
 * values.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Copy key: Ctrl+C on a tap, Ctrl+Shift+C on a hold.
 *
 * Copy and paste keys are LT(0, key) keycodes, so QMK tells a tap from a hold.
 * Layer 0 is always on, so the layer action has no effect, and
 * process_dual_function() handles every event. The basic key names the key and
 * makes the two values unique.
 */
#define COPY_KEY LT(0, KC_C)
/** Paste key: Ctrl+V on a tap, Ctrl+Shift+V on a hold */
#define PASTE_KEY LT(0, KC_V)

/** Keycodes that only the keymap code handles; QMK keeps SAFE_RANGE and up free */
enum custom_keycode {
    LOGGING_COMBO_LEFT_POSITION = SAFE_RANGE, /**< Combo key at (1,6), never sent */
    LOGGING_COMBO_RIGHT_POSITION,             /**< Combo key at (1,7), never sent */
    LOGGING_TOGGLE,                           /**< Logging combo: logging on or off */
};
