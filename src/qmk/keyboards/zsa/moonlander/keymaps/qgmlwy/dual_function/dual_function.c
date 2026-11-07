/** @file
 * Tap and hold keys of the qgmlwy keymap's dual-function keys.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"

/// Tap key and hold key of one dual-function key
typedef struct {
    uint16_t keycode; ///< Keycode of the dual-function key, for example COPY_KEY
    uint16_t tap;     ///< Key sent when the event is a tap
    uint16_t hold;    ///< Key sent when the event is a hold
} dual_function_t;

/// Tap and hold key of each dual-function key
static const dual_function_t dual_functions[] = {
    {COPY_KEY, LCTL(KC_C), LCTL(LSFT(KC_C))},
    {PASTE_KEY, LCTL(KC_V), LCTL(LSFT(KC_V))},
};

bool process_dual_function(uint16_t keycode, keyrecord_t *record) {
    for (size_t i = 0; i < ARRAY_SIZE(dual_functions); i++) {
        if (dual_functions[i].keycode != keycode) {
            continue;
        }
        uint16_t sent =
            record->tap.count > 0 ? dual_functions[i].tap : dual_functions[i].hold;
        if (record->event.pressed) {
            register_code16(sent);
        } else {
            unregister_code16(sent);
        }
        return false;
    }
    return true;
}
