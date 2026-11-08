/** @file
 * Modes in which the combos operate.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "combos/combos.h"
#include "layers/layers.h"

// #endregion

// #region Public functions

void update_combos(void) {
    if (is_combo_enabled() == layer_state_is(LAYER_NAV)) {
        combo_toggle();
    }
}

bool combos_operate(void) {
    // Special mode takes each combo keycode as one push and does nothing else with it
    if (layer_state_is(LAYER_SPECIAL)) {
        return false;
    }
    return is_combo_enabled();
}

// #endregion
