/** @file
 * Copy and paste keycodes of the qgmlwy keymap.
 *
 * Base layer and process_dual_function() both use these keycodes, so one header
 * holds their values.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Copy key: Ctrl+C on a tap, Ctrl+Shift+C on a hold.
 *
 * Each `DUAL_FUNC_*` is a layer-tap keycode, so QMK tells a tap from a hold, but
 * process_dual_function() handles every event and the layer never activates; the
 * layer numbers only make the two values unique, and one names no existing layer.
 */
#define DUAL_FUNC_0 LT(6, KC_H)
/** Paste key: Ctrl+V on a tap, Ctrl+Shift+V on a hold. */
#define DUAL_FUNC_1 LT(12, KC_T)
