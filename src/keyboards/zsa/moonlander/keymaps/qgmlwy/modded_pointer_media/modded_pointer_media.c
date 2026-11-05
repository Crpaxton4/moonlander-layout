/** @file
 * Modifier handling of the qgmlwy keymap's modified mouse and consumer keys.
 */

#include QMK_KEYBOARD_H

#include "modded_pointer_media/modded_pointer_media.h"

/**
 * Sends a modified key's modifiers apart from its base key if that is a mouse or
 * consumer key.
 *
 * @param keycode Modified keycode, in QMK's `QK_MODS` range.
 * @param record  Key event.
 * @return False for the press of a mouse or consumer key; true otherwise, so QMK
 *         goes on processing the key and on release unregisters its base key.
 */
static bool modded_key_event(uint16_t keycode, keyrecord_t *record) {
    // Mouse and consumer keys (volume, media) with modifiers work
    // inconsistently across operating systems, this makes sure that modifiers
    // are always applied to the key that was pressed.
    if (IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode)) ||
        IS_CONSUMER_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
        if (record->event.pressed) {
            add_mods(QK_MODS_GET_MODS(keycode));
            send_keyboard_report();
            wait_ms(2);
            register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
            return false;
        } else {
            wait_ms(2);
            del_mods(QK_MODS_GET_MODS(keycode));
        }
    }
    return true;
}

bool process_modded_pointer_media(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_MODS ... QK_MODS_MAX:
            return modded_key_event(keycode, record);
        default:
            return true;
    }
}
