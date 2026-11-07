/** @file
 * Keycodes of the qgmlwy keymap that QMK does not have.
 *
 * Layers and the key event hooks both use these keycodes, so one header holds their
 * values.
 */

#pragma once

#include QMK_KEYBOARD_H

/** Keycodes that only the keymap code handles; QMK keeps SAFE_RANGE and up free */
enum custom_keycode {
    LOGGING_COMBO_LEFT_POSITION = SAFE_RANGE, /**< Combo key at (1,6), never sent */
    LOGGING_COMBO_RIGHT_POSITION,             /**< Combo key at (1,7), never sent */
    LOGGING_TOGGLE,                           /**< Logging combo: logging on or off */
};
