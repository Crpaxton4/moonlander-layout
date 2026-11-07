/** @file
 * Tests of the precision hold of the nav mode, through the keymap's key event hook.
 *
 * QMK's mouse key code is not in the test build. The cases read QMK's top cursor
 * speed, which that code uses for each movement report.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "mousekey.h"

#include "keycodes/keycodes.h"
#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Top cursor speed of the keymap, in counts per movement report */
#define TOP_SPEED 20

/** Tap count that QMK gives both events of the tap of a tap-hold key */
#define ONE_TAP 1

// #endregion

// #region Data

/** Push of the hold of a tap-hold key, or of a key that is not a tap-hold key */
static keyrecord_t press = {.event = {.pressed = true}};

/** Release of the hold of a tap-hold key, or of a key that is not a tap-hold key */
static keyrecord_t release = {.event = {.pressed = false}};

/** Push of the tap of a tap-hold key */
static keyrecord_t tap_press = {
    .event = {.pressed = true},
    .tap   = {.count = ONE_TAP},
};

/** Release of the tap of a tap-hold key */
static keyrecord_t tap_release = {
    .event = {.pressed = false},
    .tap   = {.count = ONE_TAP},
};

// #endregion

// #region Static functions

static void precision_key_r6_hold_sets_one_count_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    assert_false(process_record_user(PRECISION_HOLD_KEY, &press));
    assert_int_equal(mk_max_speed, 1);
}

static void precision_key_r6_release_of_hold_restores_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    process_record_user(PRECISION_HOLD_KEY, &press);
    assert_false(process_record_user(PRECISION_HOLD_KEY, &release));
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

static void precision_key_tap_sends_space_and_keeps_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    // Tap goes on to QMK, which sends the tap key
    assert_int_equal(QK_LAYER_TAP_GET_TAP_KEYCODE(PRECISION_HOLD_KEY), KC_SPACE);
    assert_true(process_record_user(PRECISION_HOLD_KEY, &tap_press));
    assert_int_equal(mk_max_speed, TOP_SPEED);
    assert_true(process_record_user(PRECISION_HOLD_KEY, &tap_release));
    assert_int_equal(mk_max_speed, TOP_SPEED);
    assert_recorded_calls(NULL, 0);
}

static void precision_key_r6_other_keys_keep_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    assert_true(process_record_user(QK_MOUSE_CURSOR_RIGHT, &press));
    assert_true(process_record_user(QK_MOUSE_CURSOR_RIGHT, &release));
    // Space key of the default mode is not a tap-hold key
    assert_true(process_record_user(KC_SPACE, &press));
    assert_true(process_record_user(KC_SPACE, &release));
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest precision_key_cases[] = {
    cmocka_unit_test(precision_key_r6_hold_sets_one_count_top_speed),
    cmocka_unit_test(precision_key_r6_release_of_hold_restores_top_speed),
    cmocka_unit_test(precision_key_tap_sends_space_and_keeps_top_speed),
    cmocka_unit_test(precision_key_r6_other_keys_keep_top_speed),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(precision_key_cases, NULL, NULL);
}

// #endregion
