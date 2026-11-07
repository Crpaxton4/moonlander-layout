/** @file
 * Characterization tests of the settings in config.h.
 *
 * Each value has its own case, so a spec that changes one value changes only one
 * case. CONSOLE_ENABLE and COMBO_ENABLE come from rules.mk, which is not C, so no
 * case can read them. mousekey.h comes after config.h, as in the firmware, so a
 * mouse key value that config.h does not set is QMK's default.
 */

#include "config.h"
#include "layers/layers.h"
#include "mousekey.h"
#include "unit_test.h"

static void config_usb_suspend_wakeup_delay(void **state) {
    (void)state;
    assert_int_equal(USB_SUSPEND_WAKEUP_DELAY, 0);
}

static void config_mk_combined(void **state) {
    (void)state;
#ifndef MK_COMBINED
    fail_msg("MK_COMBINED is not defined");
#endif
}

static void config_mousekey_move_delta(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_MOVE_DELTA, 1);
}

static void config_mousekey_interval(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_INTERVAL, 9);
}

static void config_mousekey_wheel_delay(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_WHEEL_DELAY, 200);
}

static void config_mousekey_delay(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_DELAY, 80);
}

static void config_mousekey_max_speed(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_MAX_SPEED, 20);
}

static void config_mousekey_time_to_max(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_TIME_TO_MAX, 80);
}

static void config_mousekey_wheel_max_speed(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_WHEEL_MAX_SPEED, 6);
}

static void config_mousekey_wheel_time_to_max(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_WHEEL_TIME_TO_MAX, 1);
}

static void config_moonlander_user_leds(void **state) {
    (void)state;
#ifndef MOONLANDER_USER_LEDS
    fail_msg("MOONLANDER_USER_LEDS is not defined");
#endif
}

static void config_combo_only_from_layer(void **state) {
    (void)state;
    assert_int_equal(COMBO_ONLY_FROM_LAYER, LAYER_COMBO_POSITIONS);
}

static void config_tapping_term(void **state) {
    (void)state;
    assert_int_equal(TAPPING_TERM, 250);
}

static void config_tapping_term_per_key(void **state) {
    (void)state;
#ifndef TAPPING_TERM_PER_KEY
    fail_msg("TAPPING_TERM_PER_KEY is not defined");
#endif
}

static void config_shift_tapping_term(void **state) {
    (void)state;
    assert_int_equal(SHIFT_TAPPING_TERM, 200);
}

static void config_permissive_hold(void **state) {
    (void)state;
#ifndef PERMISSIVE_HOLD
    fail_msg("PERMISSIVE_HOLD is not defined");
#endif
}

static void config_chordal_hold(void **state) {
    (void)state;
#ifndef CHORDAL_HOLD
    fail_msg("CHORDAL_HOLD is not defined");
#endif
}

static void config_flow_tap_term(void **state) {
    (void)state;
    assert_int_equal(FLOW_TAP_TERM, 150);
}

static void config_quick_tap_term(void **state) {
    (void)state;
    assert_int_equal(QUICK_TAP_TERM, 0);
}

static void config_no_hold_on_other_key_press(void **state) {
    (void)state;
#ifdef HOLD_ON_OTHER_KEY_PRESS
    fail_msg("HOLD_ON_OTHER_KEY_PRESS is defined");
#endif
}

static void config_no_retro_tapping(void **state) {
    (void)state;
#ifdef RETRO_TAPPING
    fail_msg("RETRO_TAPPING is defined");
#endif
}

static void config_serial_number(void **state) {
    (void)state;
    assert_string_equal(SERIAL_NUMBER, "LzzVe/pjxZ7X");
}

/** Cases of this program, in the order of config.h */
static const struct CMUnitTest config_cases[] = {
    cmocka_unit_test(config_usb_suspend_wakeup_delay),
    cmocka_unit_test(config_mk_combined),
    cmocka_unit_test(config_mousekey_move_delta),
    cmocka_unit_test(config_mousekey_interval),
    cmocka_unit_test(config_mousekey_wheel_delay),
    cmocka_unit_test(config_mousekey_delay),
    cmocka_unit_test(config_mousekey_max_speed),
    cmocka_unit_test(config_mousekey_time_to_max),
    cmocka_unit_test(config_mousekey_wheel_max_speed),
    cmocka_unit_test(config_mousekey_wheel_time_to_max),
    cmocka_unit_test(config_moonlander_user_leds),
    cmocka_unit_test(config_combo_only_from_layer),
    cmocka_unit_test(config_tapping_term),
    cmocka_unit_test(config_tapping_term_per_key),
    cmocka_unit_test(config_shift_tapping_term),
    cmocka_unit_test(config_permissive_hold),
    cmocka_unit_test(config_chordal_hold),
    cmocka_unit_test(config_flow_tap_term),
    cmocka_unit_test(config_quick_tap_term),
    cmocka_unit_test(config_no_hold_on_other_key_press),
    cmocka_unit_test(config_no_retro_tapping),
    cmocka_unit_test(config_serial_number),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(config_cases, NULL, NULL);
}
