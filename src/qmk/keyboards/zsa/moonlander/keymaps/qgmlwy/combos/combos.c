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

// #endregion
