/** @file
 * Per-layer key colours of the qgmlwy keymap: the colour table and the RGB matrix
 * callbacks that show it.
 */

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "lighting/layer_colors.h"
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
#include "lighting/function_media.inc"
#include "lighting/shortcuts.inc"
#include "lighting/launcher.inc"
#include "lighting/window_manager.inc"
#include "lighting/navigation_gate.inc"
#include "lighting/text_navigation.inc"
#include "lighting/system.inc"
#include "lighting/lock.inc"
};

RGB hsv_to_rgb_with_value(HSV hsv) {
    RGB   rgb = hsv_to_rgb(hsv);
    float f   = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    return (RGB){
        (uint8_t)(f * (float)(int)rgb.r),
        (uint8_t)(f * (float)(int)rgb.g),
        (uint8_t)(f * (float)(int)rgb.b),
    };
}

void set_layer_color(uint8_t layer) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        if (!hsv.h && !hsv.s && !hsv.v) {
            rgb_matrix_set_color(i, 0, 0, 0);
        } else {
            RGB rgb = hsv_to_rgb_with_value(hsv);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
}

/**
 * Turns every LED off when the RGB matrix flags are `LED_FLAG_NONE`.
 *
 * With no flags set the RGB matrix effects skip every LED, which would otherwise
 * keep the colours they last showed.
 */
static void turn_off_leds_if_flags_none(void) {
    if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }
}

/**
 * Shows the highest active layer's colours on every RGB matrix frame.
 *
 * Does nothing while Oryx controls the LEDs. With layer colours turned off, or
 * on a layer without colours, it turns every LED off when the RGB matrix flags
 * are `LED_FLAG_NONE`.
 *
 * @return False while Oryx controls the LEDs, true otherwise.
 */
bool rgb_matrix_indicators_user(void) {
    if (rawhid_state.rgb_control) {
        return false;
    }
    uint8_t layer = get_highest_layer(layer_state);
    if (!keyboard_config.disable_layer_led && layer < LAYER_COUNT) {
        set_layer_color(layer);
    } else {
        turn_off_leds_if_flags_none();
    }

    return true;
}

/** Turns the RGB matrix on once the keyboard has started. */
void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}
