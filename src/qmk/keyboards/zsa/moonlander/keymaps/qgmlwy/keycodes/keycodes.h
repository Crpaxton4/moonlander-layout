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
 * Copy and paste keys are layer-tap keycodes, so QMK tells a tap from a hold. Their
 * layers never activate, because process_dual_function() handles every event. The
 * layer numbers only make the two values unique, and they name no layer of the
 * keymap.
 */
#define COPY_KEY LT(6, KC_H)
/** Paste key: Ctrl+V on a tap, Ctrl+Shift+V on a hold. */
#define PASTE_KEY LT(12, KC_T)
