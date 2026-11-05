/** @file
 * Per-layer key colours of the qgmlwy keymap.
 *
 * QMK calls rgb_matrix_indicators_user() on every RGB matrix frame; it paints the
 * highest active layer's colours from `ledmap` unless Oryx controls the LEDs or
 * layer colours are turned off.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Converts an HSV colour to RGB dimmed by the current RGB matrix brightness.
 *
 * Each channel of the converted colour is multiplied by the matrix brightness
 * (`rgb_matrix_config.hsv.v`) as a fraction of `UINT8_MAX` and truncated, so
 * the colour's own value and the user's brightness setting both apply.
 *
 * @param hsv Colour to convert.
 * @return The converted colour, scaled by the matrix brightness.
 */
RGB hsv_to_rgb_with_value(HSV hsv);

/**
 * Sets every LED to its colour in a layer's `ledmap` entry.
 *
 * An all-zero entry turns its LED off; any other is converted with
 * hsv_to_rgb_with_value().
 *
 * @param layer Layer whose colours to show, an index into `ledmap`.
 */
void set_layer_color(int layer);
