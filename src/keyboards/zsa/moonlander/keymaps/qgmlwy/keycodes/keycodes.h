/** @file
 * Custom keycodes of the qgmlwy keymap.
 *
 * The keymap's layers, the lighting keys and the macros all name these keycodes,
 * so every file that uses them sees the same values.
 */

#pragma once

#include QMK_KEYBOARD_H

#ifndef ZSA_SAFE_RANGE
/**
 * First keycode free for this keymap's custom keycodes.
 *
 * ZSA's defaults module declares `ZSA_SAFE_RANGE` as an enumerator, not a macro,
 * which `#ifndef` can't see, so this definition always applies and the custom
 * keycodes start at QMK's `SAFE_RANGE`.
 */
#    define ZSA_SAFE_RANGE SAFE_RANGE
#endif

/**
 * Keycodes this keymap handles itself, in process_record_user().
 */
enum custom_keycodes {
    /** Switches the RGB matrix to a solid colour. */
    RGB_SLD = ZSA_SAFE_RANGE,
    /** First of the launcher and window macros sent by process_macros(). */
    ST_MACRO_0,
    ST_MACRO_1,
    ST_MACRO_2,
    ST_MACRO_3,
    ST_MACRO_4,
    ST_MACRO_5,
    ST_MACRO_6,
    ST_MACRO_7,
    ST_MACRO_8,
    ST_MACRO_9,
    ST_MACRO_10,
    ST_MACRO_11,
    ST_MACRO_12,
    ST_MACRO_13,
    ST_MACRO_14,
    ST_MACRO_15,
    ST_MACRO_16,
    ST_MACRO_17,
    ST_MACRO_18,
    ST_MACRO_19,
    ST_MACRO_20,
    ST_MACRO_21,
    ST_MACRO_22,
    ST_MACRO_23,
    ST_MACRO_24,
    ST_MACRO_25,
    ST_MACRO_26,
    ST_MACRO_27,
    ST_MACRO_28,
    ST_MACRO_29,
    ST_MACRO_30,
    ST_MACRO_31,
    ST_MACRO_32,
    ST_MACRO_33,
    ST_MACRO_34,
    ST_MACRO_35,
    ST_MACRO_36,
    ST_MACRO_37,
    ST_MACRO_38,
    ST_MACRO_39,
    ST_MACRO_40,
    ST_MACRO_41,
};

/**
 * Copy key: Ctrl+C on a tap, Ctrl+Shift+C on a hold.
 *
 * Each `DUAL_FUNC_*` is a layer-tap keycode, so QMK tells a tap from a hold, but
 * process_dual_function() handles every event and the layer never activates; the
 * layer numbers only make the four values unique, and some name no existing layer.
 */
#define DUAL_FUNC_0 LT(6, KC_H)
/** Paste key: Ctrl+V on a tap, Ctrl+Shift+V on a hold. */
#define DUAL_FUNC_1 LT(12, KC_T)
/** Previous track on a tap, rewind on a hold. */
#define DUAL_FUNC_2 LT(5, KC_F3)
/** Next track on a tap, fast forward on a hold. */
#define DUAL_FUNC_3 LT(14, KC_F15)
