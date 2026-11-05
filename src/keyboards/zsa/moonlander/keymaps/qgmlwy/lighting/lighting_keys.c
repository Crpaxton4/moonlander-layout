/** @file
 * RGB matrix mode keys of the qgmlwy keymap.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "lighting/lighting_keys.h"
#include "oryx.h"

bool process_lighting_keys(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RGB_SLD:
            if (rawhid_state.rgb_control) {
                return false;
            }
            if (record->event.pressed) {
                rgb_matrix_mode(RGB_MATRIX_SOLID_COLOR);
            }
            return false;
        default:
            return true;
    }
}
