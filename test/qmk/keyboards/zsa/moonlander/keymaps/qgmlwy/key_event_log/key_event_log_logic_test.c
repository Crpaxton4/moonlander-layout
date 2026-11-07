/** @file
 * Tests of the logging state, the idle time and the report comparison of the key
 * event log, with no QMK code.
 */

// #region Includes

#include <stdbool.h>
#include <stdint.h>

#include "key_event_log/key_event_log_logic.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** HID key code of the A key */
#define KEY_A 0x04

/** HID key code of the B key */
#define KEY_B 0x05

/** HID key code of the C key */
#define KEY_C 0x06

/** HID key code of the D key */
#define KEY_D 0x07

/** HID key code of the E key */
#define KEY_E 0x08

/** HID key code of the F key */
#define KEY_F 0x09

/** HID key code of the 9 key */
#define KEY_9 0x26

/** HID key code of the Enter key */
#define KEY_ENTER 0x28

/** Left Shift bit of the report modifier byte */
#define LEFT_SHIFT_BIT 0x02

/** Time at which a test switches logging on */
#define START_TIME 1000

/** First time at which the idle time is over, after logging went on at START_TIME */
#define IDLE_END_TIME (START_TIME + LOGGING_IDLE_MILLISECONDS)

/** Key codes in one byte of a key bitmap */
#define KEYS_PER_BITMAP_BYTE 8

/** Bit of a key code in its byte of a key bitmap */
#define KEY_CODE_BIT(code) ((uint8_t)(1U << ((code) % KEYS_PER_BITMAP_BYTE)))

/** Shortest time step */
#define ONE_MILLISECOND 1

/** Bytes in the key bitmap of a QMK NKRO report */
#define NKRO_BITMAP_BYTES 30

// #endregion

// #region Static functions

/**
 * Sets the bit of a key code in a key bitmap, as QMK's add_key_bit().
 *
 * @param bits     Key bitmap.
 * @param key_code HID key code.
 */
static void set_key_bit(uint8_t *bits, uint8_t key_code) {
    bits[key_code / KEYS_PER_BITMAP_BYTE] |= KEY_CODE_BIT(key_code);
}

static void key_event_log_logic_r1_static_state_is_off(void **state) {
    (void)state;
    // Static storage starts as zero, as the state in key_event_log.c does
    static logging_t logging;
    assert_false(logging.is_on);
}

static void key_event_log_logic_r2_toggle_turns_on_then_off(void **state) {
    (void)state;
    logging_t logging = {.is_on = false};
    assert_int_equal(logging_toggle(&logging, START_TIME), LOGGING_ON);
    assert_true(logging.is_on);
    assert_int_equal(logging_toggle(&logging, START_TIME), LOGGING_OFF);
    assert_false(logging.is_on);
}

static void key_event_log_logic_r6_push_and_release_directions(void **state) {
    (void)state;
    assert_int_equal(key_event_direction(true), 'd');
    assert_int_equal(key_event_direction(false), 'u');
}

static void key_event_log_logic_r7_first_report_with_a_key_is_a_change(void **state) {
    (void)state;
    logging_t       logging = {.is_on = false};
    logged_report_t report  = {.keys = {KEY_A}};
    assert_true(logging_report_changed(&logging, &report));
    assert_false(logging_report_changed(&logging, &report));
}

static void key_event_log_logic_r7_modifier_change_is_a_change(void **state) {
    (void)state;
    logging_t       logging = {.is_on = false};
    logged_report_t report  = {.modifiers = 0};
    assert_false(logging_report_changed(&logging, &report));
    report.modifiers = LEFT_SHIFT_BIT;
    assert_true(logging_report_changed(&logging, &report));
}

static void key_event_log_logic_r7_bitmap_keys_in_code_order(void **state) {
    (void)state;
    uint8_t bits[NKRO_BITMAP_BYTES] = {0};
    set_key_bit(bits, KEY_9);
    set_key_bit(bits, KEY_A);
    uint8_t keys[LOGGED_REPORT_KEY_COUNT];
    keys_from_bitmap(keys, bits, sizeof(bits));
    static const uint8_t expected[LOGGED_REPORT_KEY_COUNT] = {KEY_A, KEY_9};
    assert_memory_equal(keys, expected, LOGGED_REPORT_KEY_COUNT);
}

static void key_event_log_logic_r7_bitmap_keeps_first_six_keys(void **state) {
    (void)state;
    uint8_t bits[NKRO_BITMAP_BYTES] = {0};
    for (uint8_t key_code = KEY_A; key_code <= KEY_ENTER; key_code++) {
        set_key_bit(bits, key_code);
    }
    uint8_t keys[LOGGED_REPORT_KEY_COUNT];
    keys_from_bitmap(keys, bits, sizeof(bits));
    static const uint8_t expected[LOGGED_REPORT_KEY_COUNT] = {
        KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F,
    };
    assert_memory_equal(keys, expected, LOGGED_REPORT_KEY_COUNT);
}

static void key_event_log_logic_r8_state_names(void **state) {
    (void)state;
    assert_string_equal(logging_state_name(LOGGING_ON), "on");
    assert_string_equal(logging_state_name(LOGGING_OFF), "off");
    assert_string_equal(logging_state_name(LOGGING_IDLE_OFF), "idle-off");
}

static void key_event_log_logic_r9_idle_off_after_thirty_minutes(void **state) {
    (void)state;
    logging_t logging = {.is_on = false};
    logging_toggle(&logging, START_TIME);
    assert_false(logging_idle_ended(&logging, IDLE_END_TIME - ONE_MILLISECOND));
    assert_true(logging.is_on);
    assert_true(logging_idle_ended(&logging, IDLE_END_TIME));
    assert_false(logging.is_on);
    assert_false(logging_idle_ended(&logging, IDLE_END_TIME));
}

static void key_event_log_logic_r9_push_starts_the_idle_time_again(void **state) {
    (void)state;
    logging_t logging = {.is_on = false};
    logging_toggle(&logging, START_TIME);
    const uint32_t push_time = IDLE_END_TIME - ONE_MILLISECOND;
    logging_note_push(&logging, push_time);
    assert_false(logging_idle_ended(&logging, IDLE_END_TIME));
    assert_true(logging_idle_ended(&logging, push_time + LOGGING_IDLE_MILLISECONDS));
}

static void key_event_log_logic_r9_idle_time_continues_over_timer_wrap(void **state) {
    (void)state;
    logging_t      logging    = {.is_on = false};
    const uint32_t start_time = UINT32_MAX - START_TIME;
    const uint32_t end_time   = start_time + LOGGING_IDLE_MILLISECONDS;
    logging_toggle(&logging, start_time);
    assert_false(logging_idle_ended(&logging, end_time - ONE_MILLISECOND));
    assert_true(logging_idle_ended(&logging, end_time));
}

static void key_event_log_logic_r9_no_idle_off_while_off(void **state) {
    (void)state;
    logging_t logging = {.is_on = false};
    assert_false(logging_idle_ended(&logging, LOGGING_IDLE_MILLISECONDS));
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest key_event_log_logic_cases[] = {
    cmocka_unit_test(key_event_log_logic_r1_static_state_is_off),
    cmocka_unit_test(key_event_log_logic_r2_toggle_turns_on_then_off),
    cmocka_unit_test(key_event_log_logic_r6_push_and_release_directions),
    cmocka_unit_test(key_event_log_logic_r7_first_report_with_a_key_is_a_change),
    cmocka_unit_test(key_event_log_logic_r7_modifier_change_is_a_change),
    cmocka_unit_test(key_event_log_logic_r7_bitmap_keys_in_code_order),
    cmocka_unit_test(key_event_log_logic_r7_bitmap_keeps_first_six_keys),
    cmocka_unit_test(key_event_log_logic_r8_state_names),
    cmocka_unit_test(key_event_log_logic_r9_idle_off_after_thirty_minutes),
    cmocka_unit_test(key_event_log_logic_r9_push_starts_the_idle_time_again),
    cmocka_unit_test(key_event_log_logic_r9_idle_time_continues_over_timer_wrap),
    cmocka_unit_test(key_event_log_logic_r9_no_idle_off_while_off),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(key_event_log_logic_cases, NULL, NULL);
}

// #endregion
