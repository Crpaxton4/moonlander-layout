/** @file
 * Layers and tap dance actions of the qgmlwy keymap, and the key event dispatch
 * to its feature modules.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps` and `tap_dance_actions` with `sizeof`, so both tables are defined
 * here. Each feature lives in its own module, compiled by `rules.mk`.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "lighting/lighting_keys.h"
#include "macros/macros.h"
#include "modded_pointer_media/modded_pointer_media.h"
#include "tap_dance/tap_dance.h"

/** Keys of each layer, indexed by layer, matrix row and matrix column. */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#include "layers/base.inc"
#include "layers/arrows.inc"
#include "layers/function_media.inc"
#include "layers/shortcuts.inc"
#include "layers/launcher.inc"
#include "layers/window_manager.inc"
#include "layers/navigation_gate.inc"
#include "layers/text_navigation.inc"
#include "layers/system.inc"
#include "layers/lock.inc"
};

/** QMK actions of the tap dances, indexed by their slot. */
tap_dance_action_t tap_dance_actions[] = {
    [DANCE_0] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_0, dance_0_finished, dance_0_reset),
    [DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_1_finished, dance_1_reset),
    [DANCE_2] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_2, dance_2_finished, dance_2_reset),
    [DANCE_3] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_3, dance_3_finished, dance_3_reset),
    [DANCE_4] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_4, dance_4_finished, dance_4_reset),
    [DANCE_5] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_5, dance_5_finished, dance_5_reset),
    [DANCE_6] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_6_finished, dance_6_reset),
    [DANCE_7] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_7, dance_7_finished, dance_7_reset),
    [DANCE_8] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_8, dance_8_finished, dance_8_reset),
    [DANCE_9] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_9, dance_9_finished, dance_9_reset),
    [DANCE_10] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_10, dance_10_finished, dance_10_reset),
    [DANCE_11] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_11, dance_11_finished, dance_11_reset),
    [DANCE_12] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_12, dance_12_finished, dance_12_reset),
    [DANCE_13] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_13, dance_13_finished, dance_13_reset),
    [DANCE_14] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_14, dance_14_finished, dance_14_reset),
    [DANCE_15] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_15_finished, dance_15_reset),
    [DANCE_16] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_16_finished, dance_16_reset),
};

/**
 * Hands each key event to the feature modules, in a fixed order.
 *
 * Each module handles its own keycodes and returns true for any other, so at most
 * one module acts on a key, and QMK goes on processing the key unless that module
 * returned false.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when a module fully handled the event, true otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_modded_pointer_media(keycode, record)) {
        return false;
    }
    if (!process_macros(keycode, record)) {
        return false;
    }
    if (!process_dual_function(keycode, record)) {
        return false;
    }
    return process_lighting_keys(keycode, record);
}
