/** @file
 * Lighting keys of the qgmlwy keymap.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Switches the RGB matrix to a solid colour when `RGB_SLD` is pressed.
 *
 * Does nothing while Oryx controls the LEDs.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event; only presses switch.
 * @return False for `RGB_SLD`, which this function fully handles; true for any
 *         other key.
 */
bool process_lighting_keys(uint16_t keycode, keyrecord_t *record);
