/** @file
 * Tests of the tap-hold settings of the keymap: the tapping term of each key, the
 * hand of each key for chordal hold, and the flow tap term.
 *
 * QMK's tap-hold engine is not in the test build, so the cases call the keymap's
 * callbacks directly with the values that the engine gives.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Index of the home row key to compare when the prior key is the first one */
#define SECOND_HOME_ROW_KEY 1

// #endregion

// #region Types

/** Matrix rows that hold the keys of one hand */
typedef struct {
    uint8_t first_row; /**< First matrix row */
    uint8_t last_row;  /**< Last matrix row */
    char    hand;      /**< Expected hand of each key: 'L', 'R' or '*' */
} hand_rows_t;

// #endregion

// #region Data

/** Key event of a push, as the engine gives it to the callbacks */
static keyrecord_t push = {.event = {.pressed = true}};

/** Home row keys, from the left pinky to the right pinky */
static const uint16_t home_row_mods[] = {
    LCTL_T(KC_D), LALT_T(KC_S), LGUI_T(KC_T), LSFT_T(KC_N),
    RSFT_T(KC_A), RGUI_T(KC_E), LALT_T(KC_O), RCTL_T(KC_H),
};

// #endregion

// #region Static functions

static void tap_hold_r2_tapping_term_of_ctrl_alt_and_gui_keys(void **state) {
    (void)state;
    static const uint16_t home_row_keys[] = {
        LCTL_T(KC_D), LALT_T(KC_S), LGUI_T(KC_T),
        RGUI_T(KC_E), LALT_T(KC_O), RCTL_T(KC_H),
    };
    for (size_t index = 0; index < ARRAY_SIZE(home_row_keys); index++) {
        assert_int_equal(get_tapping_term(home_row_keys[index], &push), TAPPING_TERM);
    }
}

static void tap_hold_r2_shorter_tapping_term_of_shift_keys(void **state) {
    (void)state;
    assert_int_equal(get_tapping_term(LSFT_T(KC_N), &push), SHIFT_TAPPING_TERM);
    assert_int_equal(get_tapping_term(RSFT_T(KC_A), &push), SHIFT_TAPPING_TERM);
}

/**
 * Fails the test if a key does not have the expected hand.
 *
 * @param key  Matrix position of the key.
 * @param hand Expected hand.
 */
static void assert_hand(keypos_t key, char hand) {
    if (chordal_hold_handedness(key) != hand) {
        print_error("Row %d, column %d\n", key.row, key.col);
        assert_int_equal(chordal_hold_handedness(key), hand);
    }
}

/**
 * Fails the test at the first key of the rows that does not have the expected hand.
 *
 * @param rows Rows and their expected hand.
 */
static void assert_hand_of_rows(const hand_rows_t *rows) {
    for (uint8_t row = rows->first_row; row <= rows->last_row; row++) {
        for (uint8_t column = 0; column < MATRIX_COLS; column++) {
            assert_hand((keypos_t){.row = row, .col = column}, rows->hand);
        }
    }
}

static void tap_hold_r3_left_half_keys_are_left_hand(void **state) {
    (void)state;
    static const hand_rows_t left_half = {
        .first_row = LEFT_NUMBER_ROW,
        .last_row  = LEFT_BOTTOM_ROW,
        .hand      = 'L',
    };
    assert_hand_of_rows(&left_half);
}

static void tap_hold_r3_right_half_keys_are_right_hand(void **state) {
    (void)state;
    static const hand_rows_t right_half = {
        .first_row = RIGHT_NUMBER_ROW,
        .last_row  = RIGHT_BOTTOM_ROW,
        .hand      = 'R',
    };
    assert_hand_of_rows(&right_half);
}

static void tap_hold_r5_thumb_keys_combine_with_each_hand(void **state) {
    (void)state;
    static const hand_rows_t left_thumbs = {
        .first_row = LEFT_THUMB_KEYS,
        .last_row  = LEFT_THUMB_KEYS,
        .hand      = '*',
    };
    static const hand_rows_t right_thumbs = {
        .first_row = RIGHT_THUMB_KEYS,
        .last_row  = RIGHT_THUMB_KEYS,
        .hand      = '*',
    };
    assert_hand_of_rows(&left_thumbs);
    assert_hand_of_rows(&right_thumbs);
}

/**
 * Gives the flow tap term of the home row keys after a prior key. Fails the test if
 * two home row keys have different terms. The prior key itself is not compared,
 * because a push again of the same key has its own rule (R12).
 *
 * @param prior_key Keycode of the key before the home row key.
 * @return Flow tap term of each other home row key.
 */
static uint16_t home_row_flow_tap_term_after(uint16_t prior_key) {
    uint16_t other_key = home_row_mods[0];
    if (other_key == prior_key) {
        other_key = home_row_mods[SECOND_HOME_ROW_KEY];
    }
    uint16_t first_term = get_flow_tap_term(other_key, &push, prior_key);
    for (size_t index = 0; index < ARRAY_SIZE(home_row_mods); index++) {
        uint16_t key = home_row_mods[index];
        if (key != prior_key) {
            assert_int_equal(get_flow_tap_term(key, &push, prior_key), first_term);
        }
    }
    return first_term;
}

static void tap_hold_r6_flow_tap_term_after_a_letter(void **state) {
    (void)state;
    assert_int_equal(home_row_flow_tap_term_after(KC_A), FLOW_TAP_TERM);
    assert_int_equal(home_row_flow_tap_term_after(KC_R), FLOW_TAP_TERM);
    assert_int_equal(home_row_flow_tap_term_after(KC_Z), FLOW_TAP_TERM);
}

static void tap_hold_r6_flow_tap_term_after_a_tapped_home_row_key(void **state) {
    (void)state;
    assert_int_equal(home_row_flow_tap_term_after(LSFT_T(KC_N)), FLOW_TAP_TERM);
    assert_int_equal(home_row_flow_tap_term_after(RCTL_T(KC_H)), FLOW_TAP_TERM);
}

static void tap_hold_r6_no_flow_tap_after_other_keys(void **state) {
    (void)state;
    assert_int_equal(home_row_flow_tap_term_after(KC_NO), 0);
    assert_int_equal(home_row_flow_tap_term_after(KC_SPACE), 0);
    assert_int_equal(home_row_flow_tap_term_after(KC_ENTER), 0);
    assert_int_equal(home_row_flow_tap_term_after(KC_BACKSPACE), 0);
    assert_int_equal(home_row_flow_tap_term_after(KC_DOT), 0);
    assert_int_equal(home_row_flow_tap_term_after(KC_1), 0);
}

static void tap_hold_r12_no_flow_tap_on_a_push_of_the_same_key(void **state) {
    (void)state;
    // A slow tap and a quick push again of the same key must reach the usual
    // tap-hold decision, so that a hold gives the modifier, not the letter again
    for (size_t index = 0; index < ARRAY_SIZE(home_row_mods); index++) {
        uint16_t key = home_row_mods[index];
        assert_int_equal(get_flow_tap_term(key, &push, key), 0);
    }
}

static void tap_hold_precision_hold_has_the_usual_tapping_term(void **state) {
    (void)state;
    assert_int_equal(get_tapping_term(PRECISION_HOLD_KEY, &push), TAPPING_TERM);
}

static void tap_hold_no_flow_tap_on_precision_hold_after_nav_keys(void **state) {
    (void)state;
    // Mouse keys and arrows are not letters, so a hold of L1 after them is a hold
    static const uint16_t nav_keys[] = {
        QK_MOUSE_CURSOR_UP, QK_MOUSE_WHEEL_DOWN, KC_LEFT, KC_HOME, QK_MOUSE_BUTTON_1,
    };
    for (size_t index = 0; index < ARRAY_SIZE(nav_keys); index++) {
        uint16_t prior_key = nav_keys[index];
        assert_int_equal(get_flow_tap_term(PRECISION_HOLD_KEY, &push, prior_key), 0);
    }
}

// #endregion

// #region Test runner

/** Cases of this program, in the order of the spec requirements */
static const struct CMUnitTest tap_hold_cases[] = {
    cmocka_unit_test(tap_hold_r2_tapping_term_of_ctrl_alt_and_gui_keys),
    cmocka_unit_test(tap_hold_r2_shorter_tapping_term_of_shift_keys),
    cmocka_unit_test(tap_hold_r3_left_half_keys_are_left_hand),
    cmocka_unit_test(tap_hold_r3_right_half_keys_are_right_hand),
    cmocka_unit_test(tap_hold_r5_thumb_keys_combine_with_each_hand),
    cmocka_unit_test(tap_hold_r6_flow_tap_term_after_a_letter),
    cmocka_unit_test(tap_hold_r6_flow_tap_term_after_a_tapped_home_row_key),
    cmocka_unit_test(tap_hold_r6_no_flow_tap_after_other_keys),
    cmocka_unit_test(tap_hold_r12_no_flow_tap_on_a_push_of_the_same_key),
    cmocka_unit_test(tap_hold_precision_hold_has_the_usual_tapping_term),
    cmocka_unit_test(tap_hold_no_flow_tap_on_precision_hold_after_nav_keys),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(tap_hold_cases, NULL, NULL);
}

// #endregion
