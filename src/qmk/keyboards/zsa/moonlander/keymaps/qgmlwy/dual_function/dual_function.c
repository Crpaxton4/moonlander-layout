/** @file
 * Tap and hold keys of the qgmlwy keymap's `DUAL_FUNC_*` keys.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"

/**
 * Registers a key while the key event is a press, releases it otherwise.
 *
 * @param keycode Key to register or release.
 * @param record  Key event that decides which.
 */
static void register_while_pressed(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        register_code16(keycode);
    } else {
        unregister_code16(keycode);
    }
}

/**
 * Registers or releases the tap key on a tap and the hold key on a hold.
 *
 * @param record       Key event, with its tap count.
 * @param tap_keycode  Key sent when the event is a tap.
 * @param hold_keycode Key sent when the event is a hold.
 */
static void dual_function_event(keyrecord_t *record, uint16_t tap_keycode,
                                uint16_t hold_keycode) {
    if (record->tap.count > 0) {
        register_while_pressed(tap_keycode, record);
    } else {
        register_while_pressed(hold_keycode, record);
    }
}

bool process_dual_function(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case DUAL_FUNC_0:
            dual_function_event(record, LCTL(KC_C), LCTL(LSFT(KC_C)));
            return false;
        case DUAL_FUNC_1:
            dual_function_event(record, LCTL(KC_V), LCTL(LSFT(KC_V)));
            return false;
        case DUAL_FUNC_2:
            dual_function_event(record, KC_MEDIA_PREV_TRACK, KC_MEDIA_REWIND);
            return false;
        case DUAL_FUNC_3:
            dual_function_event(record, KC_MEDIA_NEXT_TRACK, KC_MEDIA_FAST_FORWARD);
            return false;
        default:
            return true;
    }
}
