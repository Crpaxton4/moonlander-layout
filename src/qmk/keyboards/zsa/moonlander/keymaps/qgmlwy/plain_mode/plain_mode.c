/** @file
 * Plain-mode combo: plain mode on or off.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "plain_mode/plain_mode.h"
#include "special_mode/special_mode.h"

// #endregion

// #region Static functions

/** Turns on LAYER_PLAIN, after it ends each mode that changes keys */
static void start_plain_mode(void) {
    case_modes_end_all();
    // Host cursor can move in the plain mode, so no pair may stay open after it
    bracket_pairs_clear();
    // Special mode stops the combo, but no mode may stay on below the plain layer
    layer_off(LAYER_SPECIAL);
    layer_on(LAYER_PLAIN);
}

/** Ends the plain mode if it is on, or starts it */
static void toggle_plain_mode(void) {
    if (plain_mode_is_on()) {
        layer_off(LAYER_PLAIN);
        return;
    }
    start_plain_mode();
}

// #endregion

// #region Public functions

bool process_plain_mode(uint16_t keycode, keyrecord_t *record) {
    if (keycode != PLAIN_MODE_TOGGLE) {
        return true;
    } else if (!record->event.pressed) {
        return false;
    } else if (layer_state_is(LAYER_SPECIAL)) {
        // QMK finds combos on each layer, so the special mode stops the combo here
        special_mode_count_taken_push();
        return false;
    }
    toggle_plain_mode();
    return false;
}

bool plain_mode_is_on(void) {
    return layer_state_is(LAYER_PLAIN);
}

// #endregion
