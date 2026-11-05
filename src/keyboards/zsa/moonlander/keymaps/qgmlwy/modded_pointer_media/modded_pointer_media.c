/** @file
 * Modifier handling of the qgmlwy keymap's modified mouse and consumer keys.
 */

#include QMK_KEYBOARD_H

#include "modded_pointer_media/modded_pointer_media.h"

/**
 * Tells whether a basic keycode is a mouse or consumer key.
 *
 * @param basic_keycode Basic keycode, without modifiers, as
 *                      `QK_MODS_GET_BASIC_KEYCODE()` gives it.
 * @return True for a mouse or consumer key.
 */
static bool is_pointer_media(int basic_keycode) {
    return IS_MOUSE_KEYCODE(basic_keycode) || IS_CONSUMER_KEYCODE(basic_keycode);
}

/**
 * Tells whether a keycode is a modified mouse or consumer key.
 *
 * Mouse and consumer keys (volume, media) with modifiers work inconsistently
 * across operating systems, so these keys get their modifiers sent separately.
 *
 * @param keycode Keycode of the key event.
 * @return True if the keycode is in QMK's `QK_MODS` range and its base key is a
 *         mouse or consumer key.
 */
static bool is_modded_pointer_media(uint16_t keycode) {
    return IS_QK_MODS(keycode) && is_pointer_media(QK_MODS_GET_BASIC_KEYCODE(keycode));
}

/**
 * Presses a modified key: sends its modifiers in a report of their own, then
 * registers its base key.
 *
 * @param keycode Modified keycode, in QMK's `QK_MODS` range.
 * @return False, since the press is fully handled.
 */
static bool press_modded_key(uint16_t keycode) {
    add_mods(QK_MODS_GET_MODS(keycode));
    send_keyboard_report();
    wait_ms(2);
    register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
    return false;
}

/**
 * Releases a modified key's modifiers.
 *
 * @param keycode Modified keycode, in QMK's `QK_MODS` range.
 * @return True, so QMK goes on processing the release and unregisters the base
 *         key.
 */
static bool release_modded_key(uint16_t keycode) {
    wait_ms(2);
    del_mods(QK_MODS_GET_MODS(keycode));
    return true;
}

bool process_modded_pointer_media(uint16_t keycode, keyrecord_t *record) {
    if (!is_modded_pointer_media(keycode)) {
        return true;
    }
    if (record->event.pressed) {
        return press_modded_key(keycode);
    }
    return release_modded_key(keycode);
}
