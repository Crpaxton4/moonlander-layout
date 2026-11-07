/** @file
 * Plain mode of the qgmlwy keymap: LAYER_PLAIN with QWERTY keys and no tap-hold
 * key, for games and for other persons.
 *
 * Mode is a layer and not a default layer, because QMK then clears it at
 * power-up. A default layer could stay in the EEPROM.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Starts or ends the plain mode for the plain-mode combo while the special mode is
 * off.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event.
 * @return False for the plain-mode combo, which sends no key; true otherwise.
 */
bool process_plain_mode(uint16_t keycode, keyrecord_t *record);

/**
 * Tells if the plain mode is on.
 *
 * @return True while LAYER_PLAIN is on.
 */
bool plain_mode_is_on(void);
