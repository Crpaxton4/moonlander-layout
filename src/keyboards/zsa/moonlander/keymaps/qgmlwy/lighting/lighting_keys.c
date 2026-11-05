/** @file
 * RGB matrix mode keys of the qgmlwy keymap.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "lighting/lighting_keys.h"
#include "oryx.h"

/**
 * rgblight's mode number for a static single colour, RGBLIGHT_MODE_STATIC_LIGHT.
 *
 * Without rgblight, QMK maps rgblight_mode() to rgb_matrix_mode(), where this
 * number selects RGB_MATRIX_SOLID_COLOR.
 */
#define STATIC_LIGHT_MODE 1

bool process_lighting_keys(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case RGB_SLD:
            if (rawhid_state.rgb_control) {
                return false;
            }
            if (record->event.pressed) {
                rgblight_mode(STATIC_LIGHT_MODE);
            }
            return false;
        default:
            return true;
    }
}
