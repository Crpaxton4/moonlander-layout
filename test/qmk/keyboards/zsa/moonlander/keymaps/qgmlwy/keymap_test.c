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
#include "tap_dance/tap_dance.h"
#include "unit_test.h"

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

static void keymap_number_of_layers(void **state) {
    (void)state;
    assert_int_equal(keymap_layer_count(), 3);
}

static void keymap_base_left_number_row(void **state) {
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
                COPY_KEY,
            },
    };
    assert_row(LAYER_BASE, LEFT_TOP_ROW, &expected);
}

static void keymap_base_left_home_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
                KC_D,
                KC_S,
                KC_T,
                KC_N,
                KC_R,
                PASTE_KEY,
            },
    };
    assert_row(LAYER_BASE, LEFT_HOME_ROW, &expected);
}

static void keymap_base_left_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_LEFT_SHIFT,
                KC_Z,
                KC_X,
                KC_C,
                KC_V,
                KC_J,
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
                TD(DANCE_CTRL_OR_GUI),
                KC_LEFT_ALT,
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
                KC_6,
                KC_7,
                KC_8,
                KC_9,
                KC_0,
                KC_MINUS,
                KC_EQUAL,
            },
    };
    assert_row(LAYER_BASE, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_base_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                KC_NO,
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
                KC_NO,
                KC_I,
                KC_A,
                KC_E,
                KC_O,
                KC_H,
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
                KC_K,
                KC_P,
                KC_COMMA,
                KC_DOT,
                KC_SLASH,
                TD(DANCE_LEFT_BRACKETS),
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
                TD(DANCE_RIGHT_BRACKETS),
            },
    };
    assert_row(LAYER_BASE, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_base_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = TO(LAYER_ARROWS),
                [RIGHT_OUTER_THUMB]  = QK_MOUSE_BUTTON_1,
                [RIGHT_MIDDLE_THUMB] = TD(DANCE_RIGHT_CLICK_OR_WHEEL),
                [RIGHT_INNER_THUMB]  = TD(DANCE_MIDDLE_CLICK_OR_WHEEL),
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
                KC_TRANSPARENT,
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
                KC_TRANSPARENT,
            },
    };
    assert_row(LAYER_ARROWS, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_arrows_right_thumb_keys(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                [RIGHT_LARGE_THUMB]  = TO(LAYER_BASE),
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
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_NUMBER_ROW, &expected);
}

static void keymap_combo_positions_left_top_row(void **state) {
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
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_HOME_ROW, &expected);
}

static void keymap_combo_positions_left_bottom_letter_row(void **state) {
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
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
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
                [LEFT_LARGE_THUMB]  = KC_NO,
                [LEFT_OUTER_THUMB]  = KC_NO,
                [LEFT_MIDDLE_THUMB] = KC_NO,
                [LEFT_INNER_THUMB]  = KC_NO,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, LEFT_THUMB_KEYS, &expected);
}

static void keymap_combo_positions_right_number_row(void **state) {
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
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_NUMBER_ROW, &expected);
}

static void keymap_combo_positions_right_top_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                LOGGING_COMBO_RIGHT_POSITION,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_TOP_ROW, &expected);
}

static void keymap_combo_positions_right_home_row(void **state) {
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
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_HOME_ROW, &expected);
}

static void keymap_combo_positions_right_bottom_letter_row(void **state) {
    (void)state;
    static const keymap_row_t expected = {
        .keys =
            {
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
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
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
    };
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_BOTTOM_ROW, &expected);
}

static void keymap_combo_positions_right_thumb_keys(void **state) {
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
    assert_row(LAYER_COMBO_POSITIONS, RIGHT_THUMB_KEYS, &expected);
}

/** Cases of this program, in the order of `keymaps` */
static const struct CMUnitTest keymap_cases[] = {
    cmocka_unit_test(keymap_number_of_layers),
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
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(keymap_cases, NULL, NULL);
}
