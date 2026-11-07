/** @file
 * Characterization tests of the keymap grid: each key of each layer.
 *
 * Each case covers one row or one thumb cluster of one layer, so a spec that changes
 * a row changes only the case of that row.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "keymap_introspection.h"
#include "layers/layers.h"
#include "unit_test.h"

/** Number of keys from F1 to F12 */
#define FUNCTION_KEY_COUNT 12

/** Keys of one row of a layer, as `keymaps` gives them */
typedef struct {
    uint16_t keys[MATRIX_COLS]; /**< Key of each matrix column */
} keymap_row_t;

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

static void keymap_number_of_layers(void **state) {
    (void)state;
    assert_int_equal(keymap_layer_count(), 4);
}

static void keymap_function_keys_only_on_the_base_number_row(void **state) {
    (void)state;
    uint8_t function_keys = 0;
    for (uint8_t layer = 0; layer < keymap_layer_count(); layer++) {
        for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
            function_keys += function_keys_in_row(layer, row);
        }
    }
    assert_int_equal(function_keys, FUNCTION_KEY_COUNT);
    uint8_t number_row_keys = function_keys_in_row(LAYER_BASE, LEFT_NUMBER_ROW);
    number_row_keys += function_keys_in_row(LAYER_BASE, RIGHT_NUMBER_ROW);
    assert_int_equal(number_row_keys, FUNCTION_KEY_COUNT);
}

static void keymap_base_left_number_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_ESCAPE,
                KC_F1,
                KC_F2,
                KC_F3,
                KC_F4,
                KC_F5,
                KC_F6,
            },
    };
    assert_row(LAYER_BASE, LEFT_NUMBER_ROW, &expected);
}

static void keymap_base_left_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_TAB,
                KC_Q,
                KC_G,
                KC_M,
                KC_L,
                KC_W,
                SNAKE_CASE_KEY,
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
                KC_NO,
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
                QK_MOUSE_ACCELERATION_0,
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
                KC_F7,
                KC_F8,
                KC_F9,
                KC_F10,
                KC_F11,
                KC_F12,
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
                CAMEL_CASE_KEY,
                KC_Y,
                KC_F,
                KC_U,
                KC_B,
                KC_SEMICOLON,
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
                QK_MOUSE_WHEEL_UP,
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
                QK_MOUSE_CURSOR_LEFT,
                QK_MOUSE_CURSOR_UP,
                QK_MOUSE_CURSOR_DOWN,
                QK_MOUSE_CURSOR_RIGHT,
                QK_MOUSE_WHEEL_DOWN,
            },
    };
    assert_row(LAYER_BASE, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_base_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = TG(LAYER_ARROWS),
                [RIGHT_OUTER_THUMB]  = QK_MOUSE_BUTTON_1,
                [RIGHT_MIDDLE_THUMB] = QK_MOUSE_BUTTON_2,
                [RIGHT_INNER_THUMB]  = QK_MOUSE_BUTTON_3,
            },
    };
    assert_row(LAYER_BASE, RIGHT_THUMB_KEYS, &expected);
}

static void keymap_arrows_left_number_row(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_NUMBER_ROW, &expected);
}

static void keymap_arrows_left_top_row(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_TOP_ROW, &expected);
}

static void keymap_arrows_left_home_row(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_HOME_ROW, &expected);
}

static void keymap_arrows_left_bottom_letter_row(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_arrows_left_bottom_row(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_BOTTOM_ROW, &expected);
}

static void keymap_arrows_left_thumb_keys(void **state) {
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
    assert_row(LAYER_ARROWS, LEFT_THUMB_KEYS, &expected);
}

static void keymap_arrows_right_number_row(void **state) {
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
    assert_row(LAYER_ARROWS, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_arrows_right_top_row(void **state) {
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
    assert_row(LAYER_ARROWS, RIGHT_TOP_ROW, &expected);
}

static void keymap_arrows_right_home_row(void **state) {
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
    assert_row(LAYER_ARROWS, RIGHT_HOME_ROW, &expected);
}

static void keymap_arrows_right_bottom_letter_row(void **state) {
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
                KC_HOME,
            },
    };
    assert_row(LAYER_ARROWS, RIGHT_BOTTOM_LETTER_ROW, &expected);
}

static void keymap_arrows_right_bottom_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_LEFT,
                KC_UP,
                KC_DOWN,
                KC_RIGHT,
                KC_END,
            },
    };
    assert_row(LAYER_ARROWS, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_arrows_right_thumb_keys(void **state) {
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
    assert_row(LAYER_ARROWS, RIGHT_THUMB_KEYS, &expected);
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
                KC_TRANSPARENT,
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
                KC_TRANSPARENT,
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
                KC_TILDE,
                KC_AT,
                KC_CIRCUMFLEX,
                KC_PERCENT,
                KC_GRAVE,
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
                KC_EXCLAIM,
                KC_ASTERISK,
                KC_DOLLAR,
                KC_EQUAL,
                KC_AMPERSAND,
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
                KC_HASH,
                KC_PIPE,
                KC_PLUS,
                KC_MINUS,
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
                KC_NO,
                KC_NO,
                KC_NO,
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
                [LEFT_INNER_THUMB]  = KC_NO,
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

/** Cases of this program, in the order of `keymaps` */
static const struct CMUnitTest keymap_cases[] = {
    cmocka_unit_test(keymap_number_of_layers),
    cmocka_unit_test(keymap_function_keys_only_on_the_base_number_row),
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
    cmocka_unit_test(keymap_arrows_left_number_row),
    cmocka_unit_test(keymap_arrows_left_top_row),
    cmocka_unit_test(keymap_arrows_left_home_row),
    cmocka_unit_test(keymap_arrows_left_bottom_letter_row),
    cmocka_unit_test(keymap_arrows_left_bottom_row),
    cmocka_unit_test(keymap_arrows_left_thumb_keys),
    cmocka_unit_test(keymap_arrows_right_number_row),
    cmocka_unit_test(keymap_arrows_right_top_row),
    cmocka_unit_test(keymap_arrows_right_home_row),
    cmocka_unit_test(keymap_arrows_right_bottom_letter_row),
    cmocka_unit_test(keymap_arrows_right_bottom_row),
    cmocka_unit_test(keymap_arrows_right_thumb_keys),
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
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(keymap_cases, NULL, NULL);
}
