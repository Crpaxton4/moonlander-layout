/** @file
 * Key lights of the Moonlander: each key that does something shows one color, the
 * color of the mode of the highest active layer. Keys that do nothing in that mode
 * are off. The keymap tells which keys do nothing, so the lights follow each change of
 * the keymap.
 *
 * QMK calls the RGB matrix indicator hook after each frame of the effect, after the
 * keyboard and module code, and before it sends the frame to the LEDs. Thus the mode
 * color replaces the effect and the Oryx colors. QMK does not call the hook while the
 * RGB matrix is off, while the host sleeps, or while the mode is RGB_MATRIX_NONE.
 */

#pragma once

// #region Functions

/**
 * Sets the solid color effect below the mode color, without an EEPROM write. The
 * effect is never RGB_MATRIX_NONE then, so QMK calls the indicator hook. Also no
 * animation uses time for each frame.
 */
void start_layer_lights(void);

/**
 * Sets the key LEDs to the color of the highest active layer, and turns off the LEDs
 * of the keys that do nothing in the active mode
 */
void show_layer_light(void);

// #endregion
