/** @file
 * Keycodes of the qgmlwy keymap that QMK does not have.
 *
 * Layers and the key event hooks both use these keycodes, so one header holds their
 * values.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

#include "symbol_holds/symbol_holds.h"

// #endregion

// #region Macros

#ifdef DEBUG_PROFILE
/** Key at (3,0): the profiling toggle in the profiling debug build */
#    define PROFILING_TOGGLE_KEY PROFILING_TOGGLE
#else
/** Key at (3,0): no key, so the normal firmware has no profiling code */
#    define PROFILING_TOGGLE_KEY KC_NO
#endif

/**
 * Key at (1,6): a tap starts or ends snake_case, and a hold sends F6. A layer-tap
 * key cannot hold a custom keycode, so the tap key is F6 and case modes take each
 * tap.
 */
#define SNAKE_CASE_KEY KEY_HOLD(KC_F6)

/** Key at (1,7): a tap starts or ends camelCase, and a hold sends F7 */
#define CAMEL_CASE_KEY KEY_HOLD(KC_F7)

// #endregion

// #region Enumerations

/** Keycodes that only the keymap code handles; QMK keeps SAFE_RANGE and up free */
enum custom_keycode {
    LOGGING_COMBO_LEFT_POSITION = SAFE_RANGE, /**< Combo key at (1,6), never sent */
    LOGGING_COMBO_RIGHT_POSITION,             /**< Combo key at (1,7), never sent */
    LOGGING_TOGGLE,                           /**< Logging combo: logging on or off */
    SPECIAL_MODE_KEY,  /**< (2,0): special mode on, or off on the special layer */
    CONSTANT_CASE_KEY, /**< Starts or ends CONSTANT_CASE */
    PASCAL_CASE_KEY,   /**< Starts or ends PascalCase */
    PLAIN_MODE_COMBO_LEFT_POSITION,  /**< Combo key at (2,6), never sent */
    PLAIN_MODE_COMBO_RIGHT_POSITION, /**< Combo key at (2,7), never sent */
    PLAIN_MODE_TOGGLE,               /**< Plain-mode combo: plain mode on or off */
    PROFILING_TOGGLE,   /**< (3,0) in the profiling debug build: profiling on or off */
    NAV_SNAKE_CASE_KEY, /**< (1,6) in the nav mode: snake_case with no F6 hold */
    NAV_CAMEL_CASE_KEY, /**< (1,7) in the nav mode: camelCase with no F7 hold */
};

// #endregion
