/** @file
 * Mouse and consumer keys with modifiers in the qgmlwy keymap.
 *
 * Operating systems apply modifiers to mouse and consumer keys (volume, media)
 * inconsistently, so these keys send their modifiers first, in a report of their
 * own.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Sends a modified mouse or consumer key with its modifiers applied first.
 *
 * A press adds the modifiers, sends them, waits 2 ms and registers the base key;
 * a release waits 2 ms and removes the modifiers, then leaves QMK to release the
 * base key.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False for the press of a modified mouse or consumer key, which this
 *         function fully handles; true for its release and for any other key.
 */
bool process_modded_pointer_media(uint16_t keycode, keyrecord_t *record);
