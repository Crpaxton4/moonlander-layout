/** @file
 * Tests of the key lights through the keymap's RGB matrix indicator hook and its
 * start-up hook.
 *
 * Each color is a value 0xRRGGBB, as the hex colors of the spec, so a failure shows
 * the color in the same form.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "keymap_introspection.h"

#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Bit of one layer in a layer state */
#define LAYER_BIT(layer) ((layer_state_t)(1U << (layer)))

/** Layer above all keymap layers, which has no color of its own */
#define LAYER_WITHOUT_COLOR (LAYER_PLAIN + 1)

/** Color of the default mode that the owner fixed: #33bbcc light blue */
#define DEFAULT_MODE_COLOR_VALUE 0x33BBCCU

/** Color of the plain mode: white */
#define PLAIN_MODE_COLOR_VALUE 0xFFFFFFU

/**
 * Highest brightness of the RGB matrix on the Moonlander, from the ZSA keyboard data.
 * QMK does not let the brightness go above it.
 */
#define ZSA_MAXIMUM_BRIGHTNESS 175

/** Color of the default mode at ZSA_MAXIMUM_BRIGHTNESS: each channel × 175 / 255 */
#define DIMMED_DEFAULT_MODE_COLOR_VALUE 0x23808CU

/** Color of the plain mode at ZSA_MAXIMUM_BRIGHTNESS: 175 on each channel */
#define DIMMED_PLAIN_MODE_COLOR_VALUE 0xAFAFAFU

/** Bit position of the red channel in a color value */
#define RED_POSITION 16U

/** Bit position of the green channel in a color value */
#define GREEN_POSITION 8U

// #endregion

// #region Static functions

/**
 * Resets the fake and sets the active layers. The brightness is then UINT8_MAX, so
 * that each LED gets the color of the mode with no change.
 *
 * @param layers Layer state.
 */
static void set_up_layers(layer_state_t layers) {
    reset_fake_qmk();
    layer_state = layers;
}

/**
 * Sets the active layers and runs the indicator hook once, as QMK does after each
 * frame of the RGB matrix effect.
 *
 * @param layers Layer state.
 */
static void show_layers(layer_state_t layers) {
    set_up_layers(layers);
    rgb_matrix_indicators_user();
}

/**
 * Sets the active layers and the ZSA maximum brightness, then runs the indicator hook
 * once.
 *
 * @param layers Layer state.
 */
static void show_layers_at_zsa_maximum(layer_state_t layers) {
    set_up_layers(layers);
    set_fake_key_light_brightness(ZSA_MAXIMUM_BRIGHTNESS);
    rgb_matrix_indicators_user();
}

/**
 * Gives a color as one value, 0xRRGGBB.
 *
 * @param color Color.
 * @return Color value.
 */
static uint32_t color_value(rgb_t color) {
    uint32_t red   = (uint32_t)color.r << RED_POSITION;
    uint32_t green = (uint32_t)color.g << GREEN_POSITION;
    return red | green | color.b;
}

/**
 * Fails the test if a key LED does not have the expected color.
 *
 * @param expected_value Expected color value of each key LED.
 */
static void assert_all_keys(uint32_t expected_value) {
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        uint32_t value = color_value(fake_key_light_color(led));
        if (value != expected_value) {
            print_error("Key LED %d\n", led);
            assert_int_equal(value, expected_value);
        }
    }
}

/**
 * Turns the hue of a color 120° forward, at the same saturation and lightness.
 *
 * Saturation and lightness come only from the highest and the lowest channel value.
 * The hue comes from the channel order of the values, and each 120° moves this order
 * one channel forward. Thus the turn moves each value to the next channel: red to
 * green, green to blue and blue to red.
 *
 * @param color Color.
 * @return Color with the hue 120° forward.
 */
static rgb_t turn_hue_forward(rgb_t color) {
    return (rgb_t){.r = color.b, .g = color.r, .b = color.g};
}

/**
 * Turns the hue of a color 120° back, at the same saturation and lightness. Each
 * value moves to the previous channel: red to blue, green to red and blue to green.
 *
 * @param color Color.
 * @return Color with the hue 120° back.
 */
static rgb_t turn_hue_back(rgb_t color) {
    return (rgb_t){.r = color.g, .g = color.b, .b = color.r};
}

static void layer_lights_default_mode_lights_all_keys_light_blue(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    assert_all_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_nav_mode_color_is_default_hue_plus_120_degrees(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = fake_key_light_color(0);
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_NAV));
    assert_all_keys(color_value(turn_hue_forward(default_color)));
}

static void layer_lights_special_mode_color_is_default_hue_minus_120_degrees(
    void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = fake_key_light_color(0);
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_SPECIAL));
    assert_all_keys(color_value(turn_hue_back(default_color)));
}

static void layer_lights_plain_mode_lights_all_keys_white(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_PLAIN));
    assert_all_keys(PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_special_mode_over_nav_mode_shows_special_color(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_SPECIAL));
    rgb_t special_color = fake_key_light_color(0);
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_NAV) |
                LAYER_BIT(LAYER_SPECIAL));
    assert_all_keys(color_value(special_color));
}

static void layer_lights_plain_mode_over_special_mode_shows_white(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_SPECIAL) |
                LAYER_BIT(LAYER_PLAIN));
    assert_all_keys(PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_empty_layer_state_shows_default_color(void **state) {
    (void)state;
    // QMK keeps the base layer on when the layer state is 0
    show_layers(0);
    assert_all_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_layer_without_color_shows_default_color(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_WITHOUT_COLOR));
    assert_all_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_brightness_dims_default_color(void **state) {
    (void)state;
    show_layers_at_zsa_maximum(LAYER_BIT(LAYER_BASE));
    assert_all_keys(DIMMED_DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_brightness_dims_white(void **state) {
    (void)state;
    show_layers_at_zsa_maximum(LAYER_BIT(LAYER_PLAIN));
    assert_all_keys(DIMMED_PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_no_mode_is_brighter_than_zsa_maximum(void **state) {
    (void)state;
    for (uint8_t layer = 0; layer < keymap_layer_count(); layer++) {
        show_layers_at_zsa_maximum(LAYER_BIT(layer));
        rgb_t color = fake_key_light_color(0);
        assert_in_range(color.r, 0, ZSA_MAXIMUM_BRIGHTNESS);
        assert_in_range(color.g, 0, ZSA_MAXIMUM_BRIGHTNESS);
        assert_in_range(color.b, 0, ZSA_MAXIMUM_BRIGHTNESS);
    }
}

static void layer_lights_zero_brightness_turns_keys_off(void **state) {
    (void)state;
    set_up_layers(LAYER_BIT(LAYER_PLAIN));
    set_fake_key_light_brightness(0);
    rgb_matrix_indicators_user();
    assert_all_keys(0);
}

static void layer_lights_start_up_sets_solid_color_effect_without_eeprom(void **state) {
    (void)state;
    reset_fake_qmk();
    keyboard_post_init_user();
    const qmk_call_t expected[] = {
        {
            .function = CALLED_RGB_MATRIX_MODE_NOEEPROM,
            .argument = RGB_MATRIX_SOLID_COLOR,
        },
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void layer_lights_hook_stops_keyboard_indicators(void **state) {
    (void)state;
    reset_fake_qmk();
    assert_false(rgb_matrix_indicators_user());
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest layer_lights_cases[] = {
    cmocka_unit_test(layer_lights_default_mode_lights_all_keys_light_blue),
    cmocka_unit_test(layer_lights_nav_mode_color_is_default_hue_plus_120_degrees),
    cmocka_unit_test(layer_lights_special_mode_color_is_default_hue_minus_120_degrees),
    cmocka_unit_test(layer_lights_plain_mode_lights_all_keys_white),
    cmocka_unit_test(layer_lights_special_mode_over_nav_mode_shows_special_color),
    cmocka_unit_test(layer_lights_plain_mode_over_special_mode_shows_white),
    cmocka_unit_test(layer_lights_empty_layer_state_shows_default_color),
    cmocka_unit_test(layer_lights_layer_without_color_shows_default_color),
    cmocka_unit_test(layer_lights_brightness_dims_default_color),
    cmocka_unit_test(layer_lights_brightness_dims_white),
    cmocka_unit_test(layer_lights_no_mode_is_brighter_than_zsa_maximum),
    cmocka_unit_test(layer_lights_zero_brightness_turns_keys_off),
    cmocka_unit_test(layer_lights_start_up_sets_solid_color_effect_without_eeprom),
    cmocka_unit_test(layer_lights_hook_stops_keyboard_indicators),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(layer_lights_cases, NULL, NULL);
}

// #endregion
