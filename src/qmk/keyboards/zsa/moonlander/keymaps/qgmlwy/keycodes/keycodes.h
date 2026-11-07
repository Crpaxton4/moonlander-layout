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
 * Copy and paste keys are LT(0, key) keycodes, so QMK tells a tap from a hold.
 * Layer 0 is always on, so the layer action has no effect, and
 * process_dual_function() handles every event. The basic key names the key and
 * makes the two values unique.
 */
#define COPY_KEY LT(0, KC_C)
/** Paste key: Ctrl+V on a tap, Ctrl+Shift+V on a hold */
#define PASTE_KEY LT(0, KC_V)
