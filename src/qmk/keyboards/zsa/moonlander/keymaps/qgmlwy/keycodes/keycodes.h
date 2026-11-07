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
    SPECIAL_MODE_KEY,  /**< (2,0): special mode on, or off on the special layer */
    SNAKE_CASE_KEY,    /**< Starts or ends snake_case */
    CONSTANT_CASE_KEY, /**< Starts or ends CONSTANT_CASE */
    CAMEL_CASE_KEY,    /**< Starts or ends camelCase */
    PASCAL_CASE_KEY,   /**< Starts or ends PascalCase */
    PLAIN_MODE_COMBO_LEFT_POSITION,  /**< Combo key at (2,6), never sent */
    PLAIN_MODE_COMBO_RIGHT_POSITION, /**< Combo key at (2,7), never sent */
    PLAIN_MODE_TOGGLE,               /**< Plain-mode combo: plain mode on or off */
};
