/** @file
 * Characterization tests of the keymap grid: each key of each layer.
 *
 * Each case covers one row or one thumb cluster of one layer, so a spec that changes
 * a row changes only the case of that row.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "keymap_introspection.h"
#include "layers/layers.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Number of digits 1 to 0 on the numpad of the special layer */
#define NUMPAD_DIGIT_COUNT 10

/** Matrix column of the last key of a right half row, for example (3,13) */
#define LAST_RIGHT_HALF_COLUMN (MATRIX_COLS - 1)

// #endregion

// #region Types

/** Keys of one row of a layer, as `keymaps` gives them */
typedef struct {
    uint16_t keys[MATRIX_COLS]; /**< Key of each matrix column */
} keymap_row_t;

// #endregion

// #region Data

/** Small right thumb keys R2 and R3, which send no key in each mode */
static const uint8_t free_right_thumb_keys[] = {
    RIGHT_MIDDLE_THUMB,
    RIGHT_INNER_THUMB,
};

// #endregion

// #region Static functions

/**
 * Fails the test at the first key of a keymap row that is not the expected key.
 *
 * A thumb cluster uses only some columns of its row. Other columns must hold
 * KC_NO, so an expected row can leave them out.
 *
 * @param layer    Layer of the row.
 * @param row      Row in the key matrix.
 * @param expected Expected key of each column.
 */
static void assert_row(uint8_t layer, uint8_t row, const keymap_row_t *expected) {
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        uint16_t actual = keycode_at_keymap_location(layer, row, column);
        if (actual != expected->keys[column]) {
            print_error("Layer %d, row %d, column %d\n", layer, row, column);
            assert_int_equal(actual, expected->keys[column]);
        }
    }
}

/**
 * Gives the number of F1 to F12 keys in one row of a layer.
 *
 * @param layer Layer of the row.
 * @param row   Row in the key matrix.
 * @return Number of keys from KC_F1 to KC_F12.
 */
static uint8_t function_keys_in_row(uint8_t layer, uint8_t row) {
    uint8_t function_keys = 0;
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        uint16_t keycode = keycode_at_keymap_location(layer, row, column);
        if (keycode >= KC_F1 && keycode <= KC_F12) {
            function_keys++;
        }
    }
    return function_keys;
}

/**
 * Fails the test if a digit of a right-hand row of the special layer does not have
 * its Shift symbol at the same place in the left-hand block. Both blocks go in the
 * same order from left to right, so the symbols follow the order of the digits.
 *
 * @param left_row Matrix row of the left half; the right half row is the same row.
 * @return Number of digits in the right-hand row.
 */
static uint8_t assert_digit_symbols_in_order(uint8_t left_row) {
    uint8_t digits    = 0;
    uint8_t right_row = (uint8_t)(left_row + RIGHT_NUMBER_ROW);
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        uint16_t digit = keycode_at_keymap_location(LAYER_SPECIAL, right_row, column);
        if (digit < KC_1 || digit > KC_0) {
            continue;
        }
        // The matrix numbers the columns of each half from 0, so the same index
        // is the same place in both blocks
        uint16_t symbol = keycode_at_keymap_location(LAYER_SPECIAL, left_row, column);
        assert_int_equal(symbol, LSFT(digit));
        digits++;
    }
    return digits;
}

static void keymap_number_of_layers(void **state) {
    (void)state;
    assert_int_equal(keymap_layer_count(), 5);
}

static void keymap_no_plain_function_key_on_any_layer(void **state) {
    (void)state;
    // Holds of the top letter row send the F-keys, so no position sends one at once
    for (uint8_t layer = 0; layer < keymap_layer_count(); layer++) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            assert_int_equal(function_keys_in_row(layer, row), 0);
        }
    }
}

static void keymap_special_left_symbols_follow_the_numpad_digit_order(void **state) {
    (void)state;
    uint8_t digits = 0;
    for (uint8_t row = LEFT_TOP_ROW; row <= (uint8_t)LEFT_BOTTOM_ROW; row++) {
        digits += assert_digit_symbols_in_order(row);
    }
    assert_int_equal(digits, NUMPAD_DIGIT_COUNT);
}

static void keymap_base_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_BASE, LEFT_NUMBER_ROW, &expected);
}

static void keymap_base_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                LT(0, KC_TAB),
                LT(0, KC_Q),
                LT(0, KC_G),
                LT(0, KC_M),
                LT(0, KC_L),
                LT(0, KC_W),
                LT(0, KC_F6),
            },
    };
    assert_row(LAYER_BASE, LEFT_TOP_ROW, &expected);
}

static void keymap_base_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                SPECIAL_MODE_KEY,
                LCTL_T(KC_D),
                LALT_T(KC_S),
                LGUI_T(KC_T),
                LSFT_T(KC_N),
                KC_R,
                CONSTANT_CASE_KEY,
            },
    };
    assert_row(LAYER_BASE, LEFT_HOME_ROW, &expected);
}

static void keymap_base_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                PROFILING_TOGGLE_KEY,
                LT(0, KC_Z),
                LT(0, KC_X),
                LT(0, KC_C),
                LT(0, KC_V),
                LT(0, KC_J),
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_BASE, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_base_left_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_BASE, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_base_left_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_DELETE,
            },
    };
    assert_row(LAYER_BASE, LEFT_THUMB_KEYS, &expected);
}

static void keymap_base_right_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_BASE, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_base_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                LT(0, KC_F7),
                LT(0, KC_Y),
                LT(0, KC_F),
                LT(0, KC_U),
                LT(0, KC_B),
                LT(0, KC_SEMICOLON),
                KC_BACKSLASH,
            },
    };
    assert_row(LAYER_BASE, RIGHT_TOP_ROW, &expected);
}

static void keymap_base_right_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                PASCAL_CASE_KEY,
                KC_I,
                RSFT_T(KC_A),
                RGUI_T(KC_E),
                LALT_T(KC_O),
                RCTL_T(KC_H),
                KC_QUOTE,
            },
    };
    assert_row(LAYER_BASE, RIGHT_HOME_ROW, &expected);
}

static void keymap_base_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                LT(0, KC_K),
                LT(0, KC_P),
                LT(0, KC_COMMA),
                LT(0, KC_DOT),
                LT(0, KC_SLASH),
                KC_NO,
            },
    };
    assert_row(LAYER_BASE, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_base_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_BASE, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_base_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = TG(LAYER_NAV),
                [RIGHT_OUTER_THUMB]  = KC_NO,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
            },
    };
    assert_row(LAYER_BASE, RIGHT_THUMB_KEYS, &expected);
}

static void keymap_nav_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, LEFT_NUMBER_ROW, &expected);
}

static void keymap_nav_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TAB,
                KC_Q,
                KC_G,
                QK_MOUSE_CURSOR_UP,
                KC_L,
                KC_W,
                NAV_SNAKE_CASE_KEY,
            },
    };
    assert_row(LAYER_NAV, LEFT_TOP_ROW, &expected);
}

static void keymap_nav_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_D,
                QK_MOUSE_CURSOR_LEFT,
                QK_MOUSE_CURSOR_DOWN,
                QK_MOUSE_CURSOR_RIGHT,
                KC_R,
                CONSTANT_CASE_KEY,
            },
    };
    assert_row(LAYER_NAV, LEFT_HOME_ROW, &expected);
}

static void keymap_nav_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_Z,
                KC_X,
                KC_C,
                KC_V,
                KC_J,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_NAV, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_nav_left_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_NAV, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_nav_left_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [LEFT_LARGE_THUMB]  = KC_TRANSPARENT,
                [LEFT_OUTER_THUMB]  = KC_TRANSPARENT,
                [LEFT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [LEFT_INNER_THUMB]  = KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, LEFT_THUMB_KEYS, &expected);
}

static void keymap_nav_right_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_nav_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NAV_CAMEL_CASE_KEY,
                QK_MOUSE_WHEEL_UP,
                QK_MOUSE_BUTTON_1,
                KC_UP,
                QK_MOUSE_BUTTON_2,
                KC_HOME,
                KC_BACKSLASH,
            },
    };
    assert_row(LAYER_NAV, RIGHT_TOP_ROW, &expected);
}

static void keymap_nav_right_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                PASCAL_CASE_KEY,
                QK_MOUSE_WHEEL_DOWN,
                KC_LEFT,
                KC_DOWN,
                KC_RIGHT,
                KC_END,
                KC_QUOTE,
            },
    };
    assert_row(LAYER_NAV, RIGHT_HOME_ROW, &expected);
}

static void keymap_nav_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                KC_K,
                QK_MOUSE_BUTTON_4,
                QK_MOUSE_BUTTON_3,
                QK_MOUSE_BUTTON_5,
                KC_SLASH,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_nav_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_nav_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = KC_TRANSPARENT,
                [RIGHT_OUTER_THUMB]  = QK_MOUSE_ACCELERATION_0,
                [RIGHT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [RIGHT_INNER_THUMB]  = KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_NAV, RIGHT_THUMB_KEYS, &expected);
}

/**
 * Gives the key of a position in the nav mode: the key of the nav layer, or the key
 * of the base layer below a transparent key.
 *
 * @param row    Row in the key matrix.
 * @param column Column in the key matrix.
 * @return Keycode in the nav mode.
 */
static uint16_t nav_mode_key(uint8_t row, uint8_t column) {
    uint16_t keycode = keycode_at_keymap_location(LAYER_NAV, row, column);
    if (keycode != KC_TRANSPARENT) {
        return keycode;
    }
    return keycode_at_keymap_location(LAYER_BASE, row, column);
}

/**
 * Fails the test if a key sends a key in the default mode or in the nav mode.
 *
 * @param row    Row in the key matrix.
 * @param column Column in the key matrix.
 */
static void assert_key_sends_nothing_in_both_modes(uint8_t row, uint8_t column) {
    uint16_t default_key = keycode_at_keymap_location(LAYER_BASE, row, column);
    uint16_t nav_key     = nav_mode_key(row, column);
    if (default_key != KC_NO || nav_key != KC_NO) {
        print_error("Row %d, column %d\n", row, column);
        fail_msg("Default key 0x%04X, nav key 0x%04X", default_key, nav_key);
    }
}

/**
 * Fails the test at the first key of a row that sends a key in the default mode or
 * in the nav mode.
 *
 * @param row Row in the key matrix.
 */
static void assert_row_sends_nothing_in_both_modes(uint8_t row) {
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        assert_key_sends_nothing_in_both_modes(row, column);
    }
}

static void keymap_free_positions_send_nothing_in_both_modes(void **state) {
    (void)state;
    assert_row_sends_nothing_in_both_modes(LEFT_NUMBER_ROW);
    assert_row_sends_nothing_in_both_modes(RIGHT_NUMBER_ROW);
    assert_row_sends_nothing_in_both_modes(LEFT_BOTTOM_ROW);
    assert_row_sends_nothing_in_both_modes(RIGHT_BOTTOM_ROW);
    // Old wheel up position (3,13)
    uint8_t letter_row = RIGHT_BOTTOM_LETTER_ROW;
    assert_key_sends_nothing_in_both_modes(letter_row, LAST_RIGHT_HALF_COLUMN);
    for (size_t index = 0; index < ARRAY_SIZE(free_right_thumb_keys); index++) {
        uint8_t thumb = free_right_thumb_keys[index];
        assert_key_sends_nothing_in_both_modes(RIGHT_THUMB_KEYS, thumb);
    }
    // Thumb key R1 is the precision key of the nav mode only
    assert_int_equal(
        keycode_at_keymap_location(LAYER_BASE, RIGHT_THUMB_KEYS, RIGHT_OUTER_THUMB),
        KC_NO);
}

/**
 * Gives the number of mouse keys in one row of the base layer.
 *
 * @param row Row in the key matrix.
 * @return Number of mouse cursor, button, wheel and speed keys.
 */
static uint8_t mouse_keys_in_base_row(uint8_t row) {
    uint8_t mouse_keys = 0;
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        if (IS_MOUSE_KEYCODE(keycode_at_keymap_location(LAYER_BASE, row, column))) {
            mouse_keys++;
        }
    }
    return mouse_keys;
}

static void keymap_default_mode_has_no_mouse_key(void **state) {
    (void)state;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        assert_int_equal(mouse_keys_in_base_row(row), 0);
    }
}

/**
 * Fails the test at the first key of a nav layer row that is a tap-hold key.
 *
 * @param row Row in the key matrix.
 */
static void assert_no_hold_key_in_nav_row(uint8_t row) {
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        uint16_t keycode = keycode_at_keymap_location(LAYER_NAV, row, column);
        if (IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode)) {
            print_error("Row %d, column %d\n", row, column);
            fail_msg("Keycode 0x%04X is a tap-hold key", keycode);
        }
    }
}

static void keymap_nav_keys_have_no_hold(void **state) {
    (void)state;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        assert_no_hold_key_in_nav_row(row);
    }
}

static void keymap_combo_positions_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_NUMBER_ROW, &expected);
}

static void keymap_combo_positions_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                LOGGING_COMBO_LEFT_POSITION,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_TOP_ROW, &expected);
}

static void keymap_combo_positions_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                PLAIN_MODE_COMBO_LEFT_POSITION,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_HOME_ROW, &expected);
}

static void keymap_combo_positions_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_combo_positions_left_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_combo_positions_left_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [LEFT_LARGE_THUMB]  = KC_TRANSPARENT,
                [LEFT_OUTER_THUMB]  = KC_TRANSPARENT,
                [LEFT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [LEFT_INNER_THUMB]  = KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_THUMB_KEYS, &expected);
}

static void keymap_combo_positions_right_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_combo_positions_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                LOGGING_COMBO_RIGHT_POSITION,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_TOP_ROW, &expected);
}

static void keymap_combo_positions_right_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                PLAIN_MODE_COMBO_RIGHT_POSITION,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_HOME_ROW, &expected);
}

static void keymap_combo_positions_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_combo_positions_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_combo_positions_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = KC_TRANSPARENT,
                [RIGHT_OUTER_THUMB]  = KC_TRANSPARENT,
                [RIGHT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [RIGHT_INNER_THUMB]  = KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_THUMB_KEYS, &expected);
}

static void keymap_special_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_NUMBER_ROW, &expected);
}

static void keymap_special_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TAB,
                KC_GRAVE,
                KC_AMPERSAND,
                KC_ASTERISK,
                KC_LEFT_PAREN,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_TOP_ROW, &expected);
}

static void keymap_special_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                SPECIAL_MODE_KEY,
                KC_TILDE,
                KC_DOLLAR,
                KC_PERCENT,
                KC_CIRCUMFLEX,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_HOME_ROW, &expected);
}

static void keymap_special_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_EXCLAIM,
                KC_AT,
                KC_HASH,
                KC_NO,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_special_left_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_RIGHT_PAREN,
                KC_PLUS,
                KC_EQUAL,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_special_left_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_DELETE,
            },
    };
    assert_row(LAYER_SPECIAL, LEFT_THUMB_KEYS, &expected);
}

static void keymap_special_right_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_special_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_7,
                KC_8,
                KC_9,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_TOP_ROW, &expected);
}

static void keymap_special_right_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_NO,
                KC_4,
                KC_5,
                KC_6,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_HOME_ROW, &expected);
}

static void keymap_special_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_1,
                KC_2,
                KC_3,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_special_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_0,
                KC_DOT,
                KC_COMMA,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_special_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = KC_NO,
                [RIGHT_OUTER_THUMB]  = KC_NO,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
            },
    };
    assert_row(LAYER_SPECIAL, RIGHT_THUMB_KEYS, &expected);
}

static void keymap_plain_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_ESCAPE,
                KC_GRAVE,
                KC_1,
                KC_2,
                KC_3,
                KC_4,
                KC_5,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_NUMBER_ROW, &expected);
}

static void keymap_plain_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TAB,
                KC_Q,
                KC_W,
                KC_E,
                KC_R,
                KC_T,
                KC_NO,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_TOP_ROW, &expected);
}

static void keymap_plain_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_A,
                KC_S,
                KC_D,
                KC_F,
                KC_G,
                KC_NO,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_HOME_ROW, &expected);
}

static void keymap_plain_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_LEFT_SHIFT,
                KC_Z,
                KC_X,
                KC_C,
                KC_V,
                KC_B,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_plain_left_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_LEFT_CTRL,
                KC_LEFT_ALT,
                KC_LEFT_GUI,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_plain_left_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_DELETE,
            },
    };
    assert_row(LAYER_PLAIN, LEFT_THUMB_KEYS, &expected);
}

static void keymap_plain_right_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_6,
                KC_7,
                KC_8,
                KC_9,
                KC_0,
                KC_MINUS,
                KC_EQUAL,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_plain_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_Y,
                KC_U,
                KC_I,
                KC_O,
                KC_P,
                KC_BACKSLASH,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_TOP_ROW, &expected);
}

static void keymap_plain_right_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_H,
                KC_J,
                KC_K,
                KC_L,
                KC_SEMICOLON,
                KC_QUOTE,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_HOME_ROW, &expected);
}

static void keymap_plain_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                KC_N,
                KC_M,
                KC_COMMA,
                KC_DOT,
                KC_SLASH,
                KC_NO,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_plain_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_plain_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = KC_NO,
                [RIGHT_OUTER_THUMB]  = KC_NO,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
            },
    };
    assert_row(LAYER_PLAIN, RIGHT_THUMB_KEYS, &expected);
}

/**
 * Fails the test at the first key of a plain layer row that is not a plain key: a
 * transparent key, a tap-hold key, a mouse key or a keymap keycode.
 *
 * @param row Row in the key matrix.
 */
static void assert_plain_keys_in_row(uint8_t row) {
    for (uint8_t column = 0; column < MATRIX_COLS; column++) {
        uint16_t keycode  = keycode_at_keymap_location(LAYER_PLAIN, row, column);
        bool     is_plain = IS_QK_BASIC(keycode) && !IS_MOUSE_KEYCODE(keycode);
        if (keycode == KC_TRANSPARENT || !is_plain) {
            print_error("Row %d, column %d\n", row, column);
            fail_msg("Keycode 0x%04X is not a plain key", keycode);
        }
    }
}

static void keymap_plain_r3_opaque_with_plain_keys(void **state) {
    (void)state;
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        assert_plain_keys_in_row(row);
    }
}

// #endregion

// #region Test runner

/** Cases of this program, in the order of `keymaps` */
static const struct CMUnitTest keymap_cases[] = {
    cmocka_unit_test(keymap_number_of_layers),
    cmocka_unit_test(keymap_no_plain_function_key_on_any_layer),
    cmocka_unit_test(keymap_base_left_number_row),
    cmocka_unit_test(keymap_base_left_top_row),
    cmocka_unit_test(keymap_base_left_home_row),
    cmocka_unit_test(keymap_base_left_bottom_letter_row),
    cmocka_unit_test(keymap_base_left_bottom_row),
    cmocka_unit_test(keymap_base_left_thumb_keys),
    cmocka_unit_test(keymap_base_right_number_row),
    cmocka_unit_test(keymap_base_right_top_row),
    cmocka_unit_test(keymap_base_right_home_row),
    cmocka_unit_test(keymap_base_right_bottom_letter_row),
    cmocka_unit_test(keymap_base_right_bottom_row),
    cmocka_unit_test(keymap_base_right_thumb_keys),
    cmocka_unit_test(keymap_nav_left_number_row),
    cmocka_unit_test(keymap_nav_left_top_row),
    cmocka_unit_test(keymap_nav_left_home_row),
    cmocka_unit_test(keymap_nav_left_bottom_letter_row),
    cmocka_unit_test(keymap_nav_left_bottom_row),
    cmocka_unit_test(keymap_nav_left_thumb_keys),
    cmocka_unit_test(keymap_nav_right_number_row),
    cmocka_unit_test(keymap_nav_right_top_row),
    cmocka_unit_test(keymap_nav_right_home_row),
    cmocka_unit_test(keymap_nav_right_bottom_letter_row),
    cmocka_unit_test(keymap_nav_right_bottom_row),
    cmocka_unit_test(keymap_nav_right_thumb_keys),
    cmocka_unit_test(keymap_free_positions_send_nothing_in_both_modes),
    cmocka_unit_test(keymap_default_mode_has_no_mouse_key),
    cmocka_unit_test(keymap_nav_keys_have_no_hold),
    cmocka_unit_test(keymap_combo_positions_left_number_row),
    cmocka_unit_test(keymap_combo_positions_left_top_row),
    cmocka_unit_test(keymap_combo_positions_left_home_row),
    cmocka_unit_test(keymap_combo_positions_left_bottom_letter_row),
    cmocka_unit_test(keymap_combo_positions_left_bottom_row),
    cmocka_unit_test(keymap_combo_positions_left_thumb_keys),
    cmocka_unit_test(keymap_combo_positions_right_number_row),
    cmocka_unit_test(keymap_combo_positions_right_top_row),
    cmocka_unit_test(keymap_combo_positions_right_home_row),
    cmocka_unit_test(keymap_combo_positions_right_bottom_letter_row),
    cmocka_unit_test(keymap_combo_positions_right_bottom_row),
    cmocka_unit_test(keymap_combo_positions_right_thumb_keys),
    cmocka_unit_test(keymap_special_left_number_row),
    cmocka_unit_test(keymap_special_left_top_row),
    cmocka_unit_test(keymap_special_left_home_row),
    cmocka_unit_test(keymap_special_left_bottom_letter_row),
    cmocka_unit_test(keymap_special_left_bottom_row),
    cmocka_unit_test(keymap_special_left_thumb_keys),
    cmocka_unit_test(keymap_special_right_number_row),
    cmocka_unit_test(keymap_special_right_top_row),
    cmocka_unit_test(keymap_special_right_home_row),
    cmocka_unit_test(keymap_special_right_bottom_letter_row),
    cmocka_unit_test(keymap_special_right_bottom_row),
    cmocka_unit_test(keymap_special_right_thumb_keys),
    cmocka_unit_test(keymap_special_left_symbols_follow_the_numpad_digit_order),
    cmocka_unit_test(keymap_plain_left_number_row),
    cmocka_unit_test(keymap_plain_left_top_row),
    cmocka_unit_test(keymap_plain_left_home_row),
    cmocka_unit_test(keymap_plain_left_bottom_letter_row),
    cmocka_unit_test(keymap_plain_left_bottom_row),
    cmocka_unit_test(keymap_plain_left_thumb_keys),
    cmocka_unit_test(keymap_plain_right_number_row),
    cmocka_unit_test(keymap_plain_right_top_row),
    cmocka_unit_test(keymap_plain_right_home_row),
    cmocka_unit_test(keymap_plain_right_bottom_letter_row),
    cmocka_unit_test(keymap_plain_right_bottom_row),
    cmocka_unit_test(keymap_plain_right_thumb_keys),
    cmocka_unit_test(keymap_plain_r3_opaque_with_plain_keys),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(keymap_cases, NULL, NULL);
}

// #endregion
