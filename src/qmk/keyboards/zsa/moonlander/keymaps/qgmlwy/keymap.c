/** @file
 * Layers and tap dance actions of the qgmlwy keymap, and its key event hook.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps` and `tap_dance_actions` with `sizeof`, so both tables are defined
 * here. Each feature lives in its own module, compiled by `rules.mk`.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "tap_dance/tap_dance.h"

/** Keys of each layer, indexed by layer, matrix row and matrix column. */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
#include "layers/base.inc"
#include "layers/arrows.inc"
};

/** QMK actions of the tap dances, indexed by slot; each plays its `dances` row. */
tap_dance_action_t tap_dance_actions[] = {
    [DANCE_CTRL_OR_GUI]           = ACTION_TAP_DANCE_TABLE(DANCE_CTRL_OR_GUI),
    [DANCE_LEFT_BRACKETS]         = ACTION_TAP_DANCE_TABLE(DANCE_LEFT_BRACKETS),
    [DANCE_RIGHT_BRACKETS]        = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_BRACKETS),
    [DANCE_MIDDLE_CLICK_OR_WHEEL] = ACTION_TAP_DANCE_TABLE(DANCE_MIDDLE_CLICK_OR_WHEEL),
    [DANCE_RIGHT_CLICK_OR_WHEEL]  = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_CLICK_OR_WHEEL),
};

/**
 * Hands each key event to the copy and paste keys.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when process_dual_function() fully handled the event, true
 *         otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return process_dual_function(keycode, record);
}
