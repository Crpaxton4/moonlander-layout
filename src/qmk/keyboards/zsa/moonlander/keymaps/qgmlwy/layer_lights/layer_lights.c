/** @file
 * Color of each mode on the key lights, and the keys that are off.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "combos/combos.h"
#include "layer_lights/layer_lights.h"
#include "layers/layers.h"

// #endregion

// #region Macros

/**
 * Color of the default mode, #33bbcc light blue. The nav and special colors are its
 * triadic set: the same saturation and lightness, with the hue 120° forward or back.
 */
#define DEFAULT_MODE_COLOR {.r = 0x33, .g = 0xBB, .b = 0xCC}

/** Color of the nav mode, magenta: default hue plus 120° */
#define NAV_MODE_COLOR {.r = 0xCC, .g = 0x33, .b = 0xBB}

/** Color of the special mode, lime yellow: default hue minus 120° */
#define SPECIAL_MODE_COLOR {.r = 0xBB, .g = 0xCC, .b = 0x33}

/** Color of the plain mode: white */
#define PLAIN_MODE_COLOR {.r = 0xFF, .g = 0xFF, .b = 0xFF}

// #endregion

// #region Data

/** Color of each layer while it is the highest active layer */
static const rgb_t layer_colors[] = {
    [LAYER_BASE] = DEFAULT_MODE_COLOR,
    [LAYER_NAV]  = NAV_MODE_COLOR,
    // Never on; QMK only reads its keys
    [LAYER_COMBO_POSITIONS] = DEFAULT_MODE_COLOR,
    [LAYER_SPECIAL]         = SPECIAL_MODE_COLOR,
    [LAYER_PLAIN]           = PLAIN_MODE_COLOR,
};

// #endregion

// #region Static functions

/**
 * Gives the color of the highest active layer.
 *
 * @return Color of the mode.
 */
static rgb_t highest_layer_color(void) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    if (layer >= ARRAY_SIZE(layer_colors)) {
        return (rgb_t)DEFAULT_MODE_COLOR;
    }
    return layer_colors[layer];
}

/**
 * Scales a color to the brightness of the RGB matrix. QMK applies its brightness only
 * to the colors of its own effects, and keeps the brightness at the ZSA maximum or
 * less. ZSA scales the Oryx colors in the same way.
 *
 * @param color Color at the full brightness.
 * @return Color at the brightness of the RGB matrix.
 */
static rgb_t scale_to_brightness(rgb_t color) {
    unsigned brightness = rgb_matrix_get_val();
    return (rgb_t){
        .r = (uint8_t)(color.r * brightness / UINT8_MAX),
        .g = (uint8_t)(color.g * brightness / UINT8_MAX),
        .b = (uint8_t)(color.b * brightness / UINT8_MAX),
    };
}

/**
 * Tells if a key is a combo key. LAYER_COMBO_POSITIONS has a combo keycode at each
 * combo key, KC_TRANSPARENT at the other keys, and NO_PHYSICAL_KEY where the matrix
 * has no key.
 *
 * @param key Matrix position of the key.
 * @return True for a combo key.
 */
static bool is_combo_key(keypos_t key) {
    uint16_t keycode = keymap_key_to_keycode(LAYER_COMBO_POSITIONS, key);
    if (keycode == KC_TRANSPARENT) {
        return false;
    }
    return keycode != NO_PHYSICAL_KEY;
}

/**
 * Tells if a key does something in the active mode: its keycode on the active layers
 * is not KC_NO, as QMK finds it for a push, or it is the key of a combo that
 * operates.
 *
 * @param key Matrix position of the key.
 * @return True if the key does something.
 */
static bool key_is_used(keypos_t key) {
    if (keymap_key_to_keycode(layer_switch_get_layer(key), key) != KC_NO) {
        return true;
    } else if (!combos_operate()) {
        return false;
    }
    return is_combo_key(key);
}

/**
 * Turns off the LED of a key that does nothing in the active mode.
 *
 * @param row    Matrix row of the key.
 * @param column Matrix column of the key.
 */
static void turn_off_unused_key(uint8_t row, uint8_t column) {
    uint8_t  led = g_led_config.matrix_co[row][column];
    keypos_t key = {.row = row, .col = column};
    if (led == NO_LED || key_is_used(key)) {
        return;
    }
    rgb_matrix_set_color(led, RGB_OFF);
}

/** Turns off the LED of each key that does nothing in the active mode */
static void turn_off_unused_keys(void) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t column = 0; column < MATRIX_COLS; column++) {
            turn_off_unused_key(row, column);
        }
    }
}

// #endregion

// #region Public functions

void start_layer_lights(void) {
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
}

void show_layer_light(void) {
    rgb_t color = scale_to_brightness(highest_layer_color());
    rgb_matrix_set_color_all(color.r, color.g, color.b);
    turn_off_unused_keys();
}

// #endregion
