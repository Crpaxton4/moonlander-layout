/** @file
 * Modifier handling of the qgmlwy keymap's modified mouse and consumer keys.
 */

#include QMK_KEYBOARD_H

#include "modded_pointer_media/modded_pointer_media.h"

bool process_modded_pointer_media(uint16_t keycode, keyrecord_t *record) {
    if (!IS_QK_MODS(keycode)) {
        return true;
    }
    int basic_keycode = QK_MODS_GET_BASIC_KEYCODE(keycode);
    if (!IS_MOUSE_KEYCODE(basic_keycode) && !IS_CONSUMER_KEYCODE(basic_keycode)) {
        return true;
    }
    if (record->event.pressed) {
        add_mods(QK_MODS_GET_MODS(keycode));
        send_keyboard_report();
        wait_ms(2);
        register_code(basic_keycode);
        return false;
    }
    wait_ms(2);
    del_mods(QK_MODS_GET_MODS(keycode));
    return true;
}
