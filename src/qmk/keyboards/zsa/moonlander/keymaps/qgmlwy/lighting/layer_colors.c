/** @file
 * Per-layer key colours of the qgmlwy keymap: the colour table and the RGB matrix
 * callbacks that show it.
 *
 * QMK calls rgb_matrix_indicators_user() on every RGB matrix frame; it paints the
 * highest active layer's colours from `ledmap` unless Oryx controls the LEDs or
 * layer colours are turned off.
 */

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "lighting/palette.h"
#include "oryx.h"

/** RGB matrix settings, owned by QMK; `hsv.v` is the brightness. */
extern rgb_config_t rgb_matrix_config;

/**
 * Key colours of each layer as HSV triples, indexed by layer and LED.
 *
 * Each layer's colours live in `lighting/<name>.inc` in LED index order, one
 * keyboard column per line, top key first: the left half from its outer column
 * inwards, then its three thumb keys and large thumb key, then the right half the
 * same way.
 */
const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
#include "lighting/base.inc"
#include "lighting/arrows.inc"
#include "lighting/navigation_gate.inc"
#include "lighting/text_navigation.inc"
#include "lighting/system.inc"
#include "lighting/lock.inc"
};

/**
 * Shows the highest active layer's colours on every RGB matrix frame.
 *
 * Does nothing while Oryx controls the LEDs. With layer colours turned off, or
 * on a layer without colours, it turns every LED off when the RGB matrix flags
 * are `LED_FLAG_NONE`: with no flags set the RGB matrix effects skip every LED,
 * which would otherwise keep the colours they last showed. Layer colours turned off
 * count as layer LAYER_COUNT, which has no colours.
 *
 * Otherwise it sets every LED to its colour in the layer's `ledmap` entry. An
 * all-zero entry turns its LED off. Any other entry is converted to RGB, then each
 * channel is multiplied by the matrix brightness (`rgb_matrix_config.hsv.v`) as a
 * fraction of `UINT8_MAX` and truncated, so the colour's own value and the user's
 * brightness setting both apply.
 *
 * @return False while Oryx controls the LEDs, true otherwise.
 */
bool rgb_matrix_indicators_user(void) {
    if (rawhid_state.rgb_control) {
        return false;
    }
    uint8_t layer = get_highest_layer(layer_state);
    if (keyboard_config.disable_layer_led) {
        layer = LAYER_COUNT;
    }
    if (layer >= LAYER_COUNT && rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
    float f = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    for (int i = 0; layer < LAYER_COUNT && i < RGB_MATRIX_LED_COUNT; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        RGB rgb = (hsv.h | hsv.s | hsv.v) == 0 ? (RGB){0} : hsv_to_rgb(hsv);
        rgb.r   = (uint8_t)(f * (float)(int)rgb.r);
        rgb.g   = (uint8_t)(f * (float)(int)rgb.g);
        rgb.b   = (uint8_t)(f * (float)(int)rgb.b);
        rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
    }
    return true;
}

/** Turns the RGB matrix on once the keyboard has started. */
void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}
