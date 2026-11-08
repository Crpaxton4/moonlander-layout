/** @file
 * Tests of the key lights through the keymap's RGB matrix indicator hook and its
 * start-up hook.
 *
 * Each color is a value 0xRRGGBB, as the hex colors of the spec, so a failure shows
 * the color in the same form. Each case runs a housekeeping pass before the hook, as
 * QMK's main loop does, so the combos are on or off for the layer state.
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

/** Color value of a key LED that is off */
#define OFF_COLOR_VALUE 0U

/** Matrix column of the last key of a right half row, for example (3,13) */
#define LAST_RIGHT_HALF_COLUMN (MATRIX_COLS - 1)

/** Matrix column of the last key of a left half row, for example (1,6) */
#define LAST_LEFT_HALF_COLUMN (MATRIX_COLS - 1)

/** Matrix column of a home row key that sends a key in each mode: S (2,2) */
#define ALWAYS_USED_COLUMN 2

/** Number of matrix positions; each LED index has one bit in an LED set */
#define MATRIX_POSITION_COUNT (MATRIX_ROWS * MATRIX_COLS)

// #endregion

// #region Types

/** Keys next to each other in one matrix row */
typedef struct {
    uint8_t row;          /**< Matrix row, a `layer_row` */
    uint8_t first_column; /**< First matrix column, a `thumb_key` in a thumb row */
    uint8_t last_column;  /**< Last matrix column, included */
} key_run_t;

/** Expected key lights of one mode: the keys of the runs, or all other keys, are lit */
typedef struct {
    const key_run_t *runs;         /**< Keys of the set */
    size_t           run_count;    /**< Number of runs */
    bool             runs_are_lit; /**< True if only the runs are lit, else only dark */
    uint32_t         color_value;  /**< Color value of each lit key */
} key_lights_t;

// #endregion

// #region Data

/**
 * Keys of the default mode that send nothing: rows 0 and 4, (3,0), (3,13), R0, R2 and
 * R3
 */
static const key_run_t default_mode_dark_keys[] = {
    {.row = LEFT_NUMBER_ROW, .first_column = 0, .last_column = LAST_LEFT_HALF_COLUMN},
    {.row = LEFT_BOTTOM_LETTER_ROW, .first_column = 0, .last_column = 0},
    {.row = LEFT_BOTTOM_ROW, .first_column = 0, .last_column = LAST_LEFT_HALF_COLUMN},
    {.row = RIGHT_NUMBER_ROW, .first_column = 0, .last_column = LAST_RIGHT_HALF_COLUMN},
    {
        .row          = RIGHT_BOTTOM_LETTER_ROW,
        .first_column = LAST_RIGHT_HALF_COLUMN,
        .last_column  = LAST_RIGHT_HALF_COLUMN,
    },
    {.row = RIGHT_BOTTOM_ROW, .first_column = 0, .last_column = LAST_RIGHT_HALF_COLUMN},
    {
        .row          = RIGHT_THUMB_KEYS,
        .first_column = RIGHT_LARGE_THUMB,
        .last_column  = RIGHT_MIDDLE_THUMB,
    },
};

/**
 * Keys of the nav mode: the precision key D, M S T N, wheel up and down, the mouse
 * buttons, the arrows, Home, End, L1 to L3 and R1
 */
static const key_run_t nav_mode_lit_keys[] = {
    {.row = LEFT_TOP_ROW, .first_column = 3, .last_column = 3},
    {.row = LEFT_HOME_ROW, .first_column = 1, .last_column = 4},
    {.row = RIGHT_TOP_ROW, .first_column = 1, .last_column = 5},
    {.row = RIGHT_HOME_ROW, .first_column = 1, .last_column = 5},
    {.row = RIGHT_BOTTOM_LETTER_ROW, .first_column = 2, .last_column = 4},
    {
        .row          = LEFT_THUMB_KEYS,
        .first_column = LEFT_OUTER_THUMB,
        .last_column  = LEFT_INNER_THUMB,
    },
    {
        .row          = RIGHT_THUMB_KEYS,
        .first_column = RIGHT_OUTER_THUMB,
        .last_column  = RIGHT_OUTER_THUMB,
    },
};

/**
 * Keys of the special mode: Tab, the mode key, the symbols, the numpad and L0 to L3.
 * The combo keys are dark, because the special mode stops the combos.
 */
static const key_run_t special_mode_lit_keys[] = {
    {.row = LEFT_TOP_ROW, .first_column = 0, .last_column = 4},
    {.row = LEFT_HOME_ROW, .first_column = 0, .last_column = 4},
    {.row = LEFT_BOTTOM_LETTER_ROW, .first_column = 2, .last_column = 4},
    {.row = LEFT_BOTTOM_ROW, .first_column = 2, .last_column = 4},
    {.row = LEFT_THUMB_KEYS, .first_column = 0, .last_column = LAST_LEFT_HALF_COLUMN},
    {.row = RIGHT_TOP_ROW, .first_column = 2, .last_column = 4},
    {.row = RIGHT_HOME_ROW, .first_column = 2, .last_column = 4},
    {.row = RIGHT_BOTTOM_LETTER_ROW, .first_column = 2, .last_column = 4},
    {.row = RIGHT_BOTTOM_ROW, .first_column = 2, .last_column = 4},
};

/**
 * Keys of the plain mode with no key: (2,0), (3,13), (4,3), (4,4), (4,9) to (4,13)
 * and R0 to R3. The combo keys (1,6), (1,7), (2,6) and (2,7) send no key, but they
 * are lit, because the combos operate in the plain mode.
 */
static const key_run_t plain_mode_dark_keys[] = {
    {.row = LEFT_HOME_ROW, .first_column = 0, .last_column = 0},
    {.row = LEFT_BOTTOM_ROW, .first_column = 3, .last_column = 4},
    {
        .row          = RIGHT_BOTTOM_LETTER_ROW,
        .first_column = LAST_RIGHT_HALF_COLUMN,
        .last_column  = LAST_RIGHT_HALF_COLUMN,
    },
    {.row = RIGHT_BOTTOM_ROW, .first_column = 0, .last_column = LAST_RIGHT_HALF_COLUMN},
    {.row = RIGHT_THUMB_KEYS, .first_column = 0, .last_column = LAST_RIGHT_HALF_COLUMN},
};

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
 * Sets the active layers and runs one housekeeping pass and the indicator hook once,
 * as QMK does after each frame of the RGB matrix effect.
 *
 * @param layers Layer state.
 */
static void show_layers(layer_state_t layers) {
    set_up_layers(layers);
    housekeeping_task_user();
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
    housekeeping_task_user();
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
 * Gives the color of a key that sends a key in each mode, so its LED is never off.
 *
 * @return Color of the key LED of S (2,2).
 */
static rgb_t always_used_key_color(void) {
    return fake_key_light_color(
        g_led_config.matrix_co[LEFT_HOME_ROW][ALWAYS_USED_COLUMN]);
}

/**
 * Fails the test if a key LED is on with a color other than the expected color, or
 * if the key that sends a key in each mode does not have the expected color. Keys
 * that send nothing are off.
 *
 * @param expected_value Expected color value of each key LED that is on.
 */
static void assert_lit_keys(uint32_t expected_value) {
    assert_int_equal(color_value(always_used_key_color()), expected_value);
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        uint32_t value  = color_value(fake_key_light_color(led));
        bool     is_off = value == OFF_COLOR_VALUE;
        if (!is_off && value != expected_value) {
            print_error("Key LED %d\n", led);
            assert_int_equal(value, expected_value);
        }
    }
}

/**
 * Fails the test if a key LED is on.
 */
static void assert_all_keys_off(void) {
    for (uint8_t led = 0; led < RGB_MATRIX_LED_COUNT; led++) {
        uint32_t value = color_value(fake_key_light_color(led));
        if (value != OFF_COLOR_VALUE) {
            print_error("Key LED %d\n", led);
            assert_int_equal(value, OFF_COLOR_VALUE);
        }
    }
}

/**
 * Tells if a key is in one of the runs of the expected key lights.
 *
 * @param lights Expected key lights.
 * @param key    Matrix position of the key.
 * @return True if a run has the key.
 */
static bool runs_have_key(const key_lights_t *lights, keypos_t key) {
    for (size_t index = 0; index < lights->run_count; index++) {
        const key_run_t *run = &lights->runs[index];
        if (run->row == key.row && key.col >= run->first_column &&
            key.col <= run->last_column) {
            return true;
        }
    }
    return false;
}

/**
 * Fails the test if the LED of one key does not have its expected color. A matrix
 * position with no LED has no check.
 *
 * @param lights Expected key lights.
 * @param row    Matrix row of the key.
 * @param column Matrix column of the key.
 */
static void assert_key_light(const key_lights_t *lights, uint8_t row, uint8_t column) {
    uint8_t led = g_led_config.matrix_co[row][column];
    if (led == NO_LED) {
        return;
    }
    keypos_t key      = {.row = row, .col = column};
    bool     is_lit   = runs_have_key(lights, key) == lights->runs_are_lit;
    uint32_t expected = is_lit ? lights->color_value : OFF_COLOR_VALUE;
    uint32_t value    = color_value(fake_key_light_color(led));
    if (value != expected) {
        print_error("Matrix row %d, column %d, key LED %d\n", row, column, led);
        assert_int_equal(value, expected);
    }
}

/**
 * Fails the test at the first key LED that does not have its expected color.
 *
 * @param lights Expected key lights.
 */
static void assert_key_lights(const key_lights_t *lights) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t column = 0; column < MATRIX_COLS; column++) {
            assert_key_light(lights, row, column);
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
    assert_lit_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_nav_mode_color_is_default_hue_plus_120_degrees(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = always_used_key_color();
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_NAV));
    assert_lit_keys(color_value(turn_hue_forward(default_color)));
}

static void layer_lights_special_mode_color_is_default_hue_minus_120_degrees(
    void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = always_used_key_color();
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_SPECIAL));
    assert_lit_keys(color_value(turn_hue_back(default_color)));
}

static void layer_lights_plain_mode_lights_all_keys_white(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_PLAIN));
    assert_lit_keys(PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_special_mode_over_nav_mode_shows_special_color(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_SPECIAL));
    rgb_t special_color = always_used_key_color();
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_NAV) |
                LAYER_BIT(LAYER_SPECIAL));
    assert_lit_keys(color_value(special_color));
}

static void layer_lights_plain_mode_over_special_mode_shows_white(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_SPECIAL) |
                LAYER_BIT(LAYER_PLAIN));
    assert_lit_keys(PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_empty_layer_state_shows_default_color(void **state) {
    (void)state;
    // QMK keeps the base layer on when the layer state is 0
    show_layers(0);
    assert_lit_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_layer_without_color_shows_default_color(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_WITHOUT_COLOR));
    assert_lit_keys(DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_brightness_dims_default_color(void **state) {
    (void)state;
    show_layers_at_zsa_maximum(LAYER_BIT(LAYER_BASE));
    assert_lit_keys(DIMMED_DEFAULT_MODE_COLOR_VALUE);
}

static void layer_lights_brightness_dims_white(void **state) {
    (void)state;
    show_layers_at_zsa_maximum(LAYER_BIT(LAYER_PLAIN));
    assert_lit_keys(DIMMED_PLAIN_MODE_COLOR_VALUE);
}

static void layer_lights_no_mode_is_brighter_than_zsa_maximum(void **state) {
    (void)state;
    for (uint8_t layer = 0; layer < keymap_layer_count(); layer++) {
        show_layers_at_zsa_maximum(LAYER_BIT(layer));
        rgb_t color = always_used_key_color();
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
    assert_all_keys_off();
}

static void layer_lights_led_map_has_one_led_for_each_key(void **state) {
    (void)state;
    bool    led_is_used[RGB_MATRIX_LED_COUNT] = {false};
    uint8_t key_leds                          = 0;
    for (uint8_t position = 0; position < MATRIX_POSITION_COUNT; position++) {
        uint8_t led =
            g_led_config.matrix_co[position / MATRIX_COLS][position % MATRIX_COLS];
        if (led == NO_LED) {
            continue;
        }
        assert_in_range(led, 0, RGB_MATRIX_LED_COUNT - 1);
        assert_false(led_is_used[led]);
        led_is_used[led] = true;
        key_leds++;
    }
    assert_int_equal(key_leds, RGB_MATRIX_LED_COUNT);
}

static void layer_lights_default_mode_turns_off_keys_that_send_nothing(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    const key_lights_t expected = {
        .runs         = default_mode_dark_keys,
        .run_count    = ARRAY_SIZE(default_mode_dark_keys),
        .runs_are_lit = false,
        .color_value  = DEFAULT_MODE_COLOR_VALUE,
    };
    assert_key_lights(&expected);
}

static void layer_lights_nav_mode_lights_only_the_nav_keys(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = always_used_key_color();
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_NAV));
    const key_lights_t expected = {
        .runs         = nav_mode_lit_keys,
        .run_count    = ARRAY_SIZE(nav_mode_lit_keys),
        .runs_are_lit = true,
        .color_value  = color_value(turn_hue_forward(default_color)),
    };
    assert_key_lights(&expected);
}

static void layer_lights_special_mode_turns_off_disabled_keys(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE));
    rgb_t default_color = always_used_key_color();
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_SPECIAL));
    const key_lights_t expected = {
        .runs         = special_mode_lit_keys,
        .run_count    = ARRAY_SIZE(special_mode_lit_keys),
        .runs_are_lit = true,
        .color_value  = color_value(turn_hue_back(default_color)),
    };
    assert_key_lights(&expected);
}

static void layer_lights_plain_mode_turns_off_keys_with_no_key(void **state) {
    (void)state;
    show_layers(LAYER_BIT(LAYER_BASE) | LAYER_BIT(LAYER_PLAIN));
    const key_lights_t expected = {
        .runs         = plain_mode_dark_keys,
        .run_count    = ARRAY_SIZE(plain_mode_dark_keys),
        .runs_are_lit = false,
        .color_value  = PLAIN_MODE_COLOR_VALUE,
    };
    assert_key_lights(&expected);
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
    cmocka_unit_test(layer_lights_led_map_has_one_led_for_each_key),
    cmocka_unit_test(layer_lights_default_mode_turns_off_keys_that_send_nothing),
    cmocka_unit_test(layer_lights_nav_mode_lights_only_the_nav_keys),
    cmocka_unit_test(layer_lights_special_mode_turns_off_disabled_keys),
    cmocka_unit_test(layer_lights_plain_mode_turns_off_keys_with_no_key),
    cmocka_unit_test(layer_lights_start_up_sets_solid_color_effect_without_eeprom),
    cmocka_unit_test(layer_lights_hook_stops_keyboard_indicators),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(layer_lights_cases, NULL, NULL);
}

// #endregion
