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
#include "modded_pointer_media/modded_pointer_media.h"
#include "tap_dance/tap_dance.h"

/** Keys of each layer, indexed by layer, matrix row and matrix column. */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#include "layers/base.inc"
#include "layers/arrows.inc"
};

/** QMK actions of the tap dances, indexed by slot; each plays its `dances` row. */
tap_dance_action_t tap_dance_actions[] = {
    [DANCE_0] = ACTION_TAP_DANCE_TABLE(DANCE_0),
    [DANCE_1] = ACTION_TAP_DANCE_TABLE(DANCE_1),
    [DANCE_2] = ACTION_TAP_DANCE_TABLE(DANCE_2),
    [DANCE_3] = ACTION_TAP_DANCE_TABLE(DANCE_3),
    [DANCE_4] = ACTION_TAP_DANCE_TABLE(DANCE_4),
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
    return process_dual_function(keycode, record);
}
