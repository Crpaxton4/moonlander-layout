/** @file
 * Tap and hold keys of the qgmlwy keymap's `DUAL_FUNC_*` keys.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"

/** Tap key and hold key of one `DUAL_FUNC_*` key. */
typedef struct {
    /** The `DUAL_FUNC_*` keycode. */
    uint16_t keycode;
    /** Key sent when the event is a tap. */
    uint16_t tap;
    /** Key sent when the event is a hold. */
    uint16_t hold;
} dual_function_t;

/** Tap and hold key of each `DUAL_FUNC_*` key. */
static const dual_function_t dual_functions[] = {
    {DUAL_FUNC_0, LCTL(KC_C), LCTL(LSFT(KC_C))},
    {DUAL_FUNC_1, LCTL(KC_V), LCTL(LSFT(KC_V))},
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
