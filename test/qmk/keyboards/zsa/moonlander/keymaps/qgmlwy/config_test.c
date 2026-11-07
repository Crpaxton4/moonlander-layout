/** @file
 * Characterization tests of the settings in config.h.
 *
 * Each value has its own case, so a spec that changes one value changes only one
 * case. CONSOLE_ENABLE and COMBO_ENABLE come from rules.mk, which is not C, so no
 * case can read them.
 */

#include "config.h"
#include "layers/layers.h"
#include "unit_test.h"

static void config_usb_suspend_wakeup_delay(void **state) {
    (void)state;
    assert_int_equal(USB_SUSPEND_WAKEUP_DELAY, 0);
}

static void config_mousekey_interval(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_INTERVAL, 10);
}

static void config_mousekey_wheel_delay(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_WHEEL_DELAY, 200);
}

static void config_mousekey_delay(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_DELAY, 0);
}

static void config_mousekey_max_speed(void **state) {
    (void)state;
    assert_int_equal(MOUSEKEY_MAX_SPEED, 3);
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

static void config_serial_number(void **state) {
    (void)state;
    assert_string_equal(SERIAL_NUMBER, "LzzVe/pjxZ7X");
}

/** Cases of this program, in the order of config.h */
static const struct CMUnitTest config_cases[] = {
    cmocka_unit_test(config_usb_suspend_wakeup_delay),
    cmocka_unit_test(config_mousekey_interval),
    cmocka_unit_test(config_mousekey_wheel_delay),
    cmocka_unit_test(config_mousekey_delay),
    cmocka_unit_test(config_mousekey_max_speed),
    cmocka_unit_test(config_mousekey_time_to_max),
    cmocka_unit_test(config_mousekey_wheel_max_speed),
    cmocka_unit_test(config_mousekey_wheel_time_to_max),
    cmocka_unit_test(config_moonlander_user_leds),
    cmocka_unit_test(config_combo_only_from_layer),
    cmocka_unit_test(config_serial_number),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(config_cases, NULL, NULL);
}
