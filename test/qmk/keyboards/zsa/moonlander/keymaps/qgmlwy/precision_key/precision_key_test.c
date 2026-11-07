/** @file
 * Tests of the precision key of the nav mode, through the keymap's key event hook.
 *
 * QMK's mouse key code is not in the test build. The cases read QMK's top cursor
 * speed, which that code uses for each movement report.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "mousekey.h"

#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Top cursor speed of the keymap, in counts per movement report */
#define TOP_SPEED 20

// #endregion

// #region Data

/** Press of a key */
static keyrecord_t press = {.event = {.pressed = true}};

/** Release of a key */
static keyrecord_t release = {.event = {.pressed = false}};

// #endregion

// #region Static functions

static void precision_key_r6_push_sets_one_count_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    assert_false(process_record_user(QK_MOUSE_ACCELERATION_0, &press));
    assert_int_equal(mk_max_speed, 1);
}

static void precision_key_r6_release_restores_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    process_record_user(QK_MOUSE_ACCELERATION_0, &press);
    assert_false(process_record_user(QK_MOUSE_ACCELERATION_0, &release));
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

static void precision_key_r6_other_keys_keep_top_speed(void **state) {
    (void)state;
    reset_fake_qmk();
    mk_max_speed = TOP_SPEED;
    assert_true(process_record_user(QK_MOUSE_CURSOR_RIGHT, &press));
    assert_true(process_record_user(QK_MOUSE_CURSOR_RIGHT, &release));
    // Thumb key L1 is Space in both modes
    assert_true(process_record_user(KC_SPACE, &press));
    assert_true(process_record_user(KC_SPACE, &release));
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest precision_key_cases[] = {
    cmocka_unit_test(precision_key_r6_push_sets_one_count_top_speed),
    cmocka_unit_test(precision_key_r6_release_restores_top_speed),
    cmocka_unit_test(precision_key_r6_other_keys_keep_top_speed),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(precision_key_cases, NULL, NULL);
}

// #endregion
