/** @file
 * Tests of the key event log through the keymap hooks: the logging combo, the
 * records on the console and the reports to the host.
 *
 * Logging state stays from one case to the next when the program runs all its
 * cases, so each case first sets the state that it needs.
 */

#include QMK_KEYBOARD_H

#include "combos/combos.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "keymap_introspection.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Matrix row of a test key */
#define TEST_KEY_ROW 2

/** Matrix column of a test key */
#define TEST_KEY_COLUMN 3

/** Time in ms between two key events of a test */
#define EVENT_INTERVAL 1234

/** Left Shift bit of the report modifier byte */
#define LEFT_SHIFT_BIT 0x02

/** Left Ctrl bit of the report modifier byte */
#define LEFT_CTRL_BIT 0x01

/** Shortest time step */
#define ONE_MILLISECOND 1

/** Layer state with one layer on */
#define ONLY_LAYER(layer) ((layer_state_t)(1U << (layer)))

/** Tap count of a tapped key */
#define ONE_TAP 1

/** Bit of key code 0x26, the 9 key, in byte 4 of an NKRO key bitmap */
#define KEY_9_BIT 0x40

/** Byte of an NKRO key bitmap that holds key codes 0x20 to 0x27 */
#define KEY_9_BYTE 4

/**
 * Sends a push or a release of a key to process_record_user().
 *
 * @param keycode Keycode after layer resolution.
 * @param pressed True for a push.
 * @return Result of process_record_user().
 */
static bool send_key_event(uint16_t keycode, bool pressed) {
    keyrecord_t record = {
        .event =
            {
                .key     = {.row = TEST_KEY_ROW, .col = TEST_KEY_COLUMN},
                .pressed = pressed,
            },
    };
    return process_record_user(keycode, &record);
}

/** Pushes and releases the logging combo, as QMK sends the combo keycode */
static void push_logging_combo(void) {
    send_key_event(LOGGING_TOGGLE, true);
    send_key_event(LOGGING_TOGGLE, false);
}

/**
 * Sets logging on or off, then empties the console text and the call log.
 *
 * Logging goes on at time 0, so the idle time starts at 0. A housekeeping pass puts
 * the logging host driver in place.
 *
 * @param on True for logging on.
 */
static void start_with_logging(bool on) {
    reset_fake_qmk();
    if (key_event_log_is_on()) {
        push_logging_combo();
    }
    if (on) {
        push_logging_combo();
    }
    reset_fake_qmk();
    housekeeping_task_user();
    assert_int_equal(key_event_log_is_on(), on);
    assert_console_text("");
}

/**
 * Sends a 6-key report through the host driver, as QMK sends each report.
 *
 * @param modifiers Modifier byte.
 * @param first_key Key code in the first slot; the other slots are 0.
 */
static void send_test_report(uint8_t modifiers, uint8_t first_key) {
    report_keyboard_t report = {.mods = modifiers, .keys = {first_key}};
    host_get_driver()->send_keyboard(&report);
}

static void key_event_log_r1_off_at_power_up(void **state) {
    (void)state;
    reset_fake_qmk();
    assert_false(key_event_log_is_on());
    housekeeping_task_user();
    assert_true(send_key_event(KC_A, true));
    send_test_report(0, KC_A);
    assert_console_text("");
}

static void key_event_log_r2_combo_switches_logging_on(void **state) {
    (void)state;
    start_with_logging(false);
    advance_fake_clock(EVENT_INTERVAL);
    send_key_event(LOGGING_TOGGLE, true);
    assert_true(key_event_log_is_on());
    assert_console_text("KL,S,1234,on\n");
}

static void key_event_log_r2_combo_switches_logging_off(void **state) {
    (void)state;
    start_with_logging(true);
    advance_fake_clock(EVENT_INTERVAL);
    send_key_event(LOGGING_TOGGLE, true);
    assert_false(key_event_log_is_on());
    assert_console_text("KL,I,1234,d,2,3,0,7E42,00,00,0,0\n"
                        "KL,S,1234,off\n");
}

static void key_event_log_r3_combo_keys(void **state) {
    (void)state;
    assert_int_equal(combo_count(), 1);
    const combo_t *combo = combo_get(LOGGING_COMBO);
    assert_int_equal(combo->keys[0], LOGGING_COMBO_LEFT_POSITION);
    assert_int_equal(combo->keys[1], LOGGING_COMBO_RIGHT_POSITION);
    assert_int_equal(combo->keys[2], COMBO_END);
    assert_int_equal(combo->keycode, LOGGING_TOGGLE);
}

static void key_event_log_r3_combo_sends_no_key(void **state) {
    (void)state;
    start_with_logging(false);
    assert_false(send_key_event(LOGGING_TOGGLE, true));
    assert_false(send_key_event(LOGGING_TOGGLE, false));
    assert_recorded_calls(NULL, 0);
}

static void key_event_log_r6_input_record_of_a_push(void **state) {
    (void)state;
    start_with_logging(true);
    advance_fake_clock(EVENT_INTERVAL);
    layer_state = ONLY_LAYER(LAYER_ARROWS);
    set_fake_modifiers(LEFT_SHIFT_BIT);
    set_fake_one_shot_modifiers(LEFT_CTRL_BIT);
    keyrecord_t record = {
        .event =
            {
                .key     = {.row = TEST_KEY_ROW, .col = TEST_KEY_COLUMN},
                .pressed = true,
            },
        .tap = {.count = ONE_TAP, .interrupted = true},
    };
    assert_true(process_record_user(KC_LEFT, &record));
    assert_console_text("KL,I,1234,d,2,3,1,0050,02,01,1,1\n");
}

static void key_event_log_r6_input_record_of_a_release(void **state) {
    (void)state;
    start_with_logging(true);
    advance_fake_clock(EVENT_INTERVAL);
    assert_true(send_key_event(KC_A, false));
    assert_console_text("KL,I,1234,u,2,3,0,0004,00,00,0,0\n");
}

static void key_event_log_r6_input_record_of_a_copy_key(void **state) {
    (void)state;
    start_with_logging(true);
    assert_false(send_key_event(COPY_KEY, true));
    assert_console_text("KL,I,0,d,2,3,0,4006,00,00,0,0\n");
}

static void key_event_log_r7_output_record_of_a_changed_report(void **state) {
    (void)state;
    start_with_logging(true);
    send_test_report(0, 0);
    reset_fake_qmk();
    housekeeping_task_user();
    advance_fake_clock(EVENT_INTERVAL);
    send_test_report(0, KC_A);
    send_test_report(0, KC_A);
    send_test_report(0, 0);
    assert_console_text("KL,O,1234,00,04,00,00,00,00,00\n"
                        "KL,O,1234,00,00,00,00,00,00,00\n");
    static const qmk_call_t expected[] = {
        {.function = CALLED_SEND_KEYBOARD, .argument = KC_A},
        {.function = CALLED_SEND_KEYBOARD, .argument = KC_A},
        {.function = CALLED_SEND_KEYBOARD, .argument = 0},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void key_event_log_r7_output_record_of_an_nkro_report(void **state) {
    (void)state;
    start_with_logging(true);
    report_nkro_t report    = {.mods = LEFT_SHIFT_BIT};
    report.bits[KEY_9_BYTE] = KEY_9_BIT;
    host_get_driver()->send_nkro(&report);
    assert_console_text("KL,O,0,02,26,00,00,00,00,00\n");
    static const qmk_call_t expected[] = {
        {.function = CALLED_SEND_NKRO, .argument = LEFT_SHIFT_BIT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void key_event_log_r9_idle_off_after_thirty_minutes(void **state) {
    (void)state;
    start_with_logging(true);
    advance_fake_clock(LOGGING_IDLE_MILLISECONDS - ONE_MILLISECOND);
    housekeeping_task_user();
    assert_true(key_event_log_is_on());
    advance_fake_clock(ONE_MILLISECOND);
    housekeeping_task_user();
    assert_false(key_event_log_is_on());
    assert_console_text("KL,S,1800000,idle-off\n");
}

static void key_event_log_r9_push_starts_the_idle_time_again(void **state) {
    (void)state;
    start_with_logging(true);
    advance_fake_clock(EVENT_INTERVAL);
    send_key_event(KC_A, true);
    advance_fake_clock(LOGGING_IDLE_MILLISECONDS - ONE_MILLISECOND);
    housekeeping_task_user();
    assert_true(key_event_log_is_on());
}

static void key_event_log_r10_no_records_while_off(void **state) {
    (void)state;
    start_with_logging(false);
    assert_true(send_key_event(KC_A, true));
    send_test_report(0, KC_A);
    assert_true(send_key_event(KC_A, false));
    send_test_report(0, 0);
    assert_console_text("");
}

static void key_event_log_r11_reports_reach_the_host_while_off(void **state) {
    (void)state;
    start_with_logging(false);
    send_test_report(0, KC_A);
    static const qmk_call_t expected[] = {
        {.function = CALLED_SEND_KEYBOARD, .argument = KC_A},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void key_event_log_r11_driver_stays_in_place(void **state) {
    (void)state;
    start_with_logging(true);
    host_driver_t *logging_driver = host_get_driver();
    housekeeping_task_user();
    assert_ptr_equal(host_get_driver(), logging_driver);
    send_test_report(0, KC_A);
    static const qmk_call_t expected[] = {
        {.function = CALLED_SEND_KEYBOARD, .argument = KC_A},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/** Cases of this program; the power-up case is first, for a run of all cases */
static const struct CMUnitTest key_event_log_cases[] = {
    cmocka_unit_test(key_event_log_r1_off_at_power_up),
    cmocka_unit_test(key_event_log_r2_combo_switches_logging_on),
    cmocka_unit_test(key_event_log_r2_combo_switches_logging_off),
    cmocka_unit_test(key_event_log_r3_combo_keys),
    cmocka_unit_test(key_event_log_r3_combo_sends_no_key),
    cmocka_unit_test(key_event_log_r6_input_record_of_a_push),
    cmocka_unit_test(key_event_log_r6_input_record_of_a_release),
    cmocka_unit_test(key_event_log_r6_input_record_of_a_copy_key),
    cmocka_unit_test(key_event_log_r7_output_record_of_a_changed_report),
    cmocka_unit_test(key_event_log_r7_output_record_of_an_nkro_report),
    cmocka_unit_test(key_event_log_r9_idle_off_after_thirty_minutes),
    cmocka_unit_test(key_event_log_r9_push_starts_the_idle_time_again),
    cmocka_unit_test(key_event_log_r10_no_records_while_off),
    cmocka_unit_test(key_event_log_r11_reports_reach_the_host_while_off),
    cmocka_unit_test(key_event_log_r11_driver_stays_in_place),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(key_event_log_cases, NULL, NULL);
}
