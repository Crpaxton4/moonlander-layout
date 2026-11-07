/** @file
 * Tests of the profiling debug build through the keymap hooks: the toggle key, the
 * measured calls of the record handler table and the records on the console.
 *
 * Fake profiling hardware gives each measured call the same number of cycles.
 * Profiling state stays from one case to the next when the program runs all its
 * cases, so each case first sets the state that it needs.
 */

#include QMK_KEYBOARD_H

#include "matrix.h"

#include "case_modes/case_modes.h"
#include "fake_profiling_hardware.h"
#include "keycodes/keycodes.h"
#include "keymap_introspection.h"
#include "layers/layers.h"
#include "profiling/profiling.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Cycles that the fake gives each measured call */
#define CALL_CYCLES 40

/** Free main stack bytes that the fake gives */
#define FREE_STACK_BYTES 1000

/** Time in ms at which the clock record case pushes the toggle key */
#define TOGGLE_TIME 1234

/** Shortest time step */
#define ONE_MILLISECOND 1

/** Matrix column of the toggle key in LEFT_BOTTOM_ROW */
#define TOGGLE_KEY_COLUMN 0

/** Layer state with one layer on */
#define ONLY_LAYER(layer) ((layer_state_t)(1U << (layer)))

/**
 * Records of a print interval that ends at 5000 ms, with one tap of a key that each
 * record handler passes on, and three matrix scans
 */
static const char one_tap_interval_text[] = "KL,P,5000,record_user,2,80,40\n"
                                            "KL,P,5000,key_log,2,80,40\n"
                                            "KL,P,5000,plain_mode,2,80,40\n"
                                            "KL,P,5000,case_modes,2,80,40\n"
                                            "KL,P,5000,bracket_pairs,2,80,40\n"
                                            "KL,P,5000,special_mode,2,80,40\n"
                                            "KL,P,5000,symbol_holds,2,80,40\n"
                                            "KL,P,5000,precision_key,2,80,40\n"
                                            "KL,R,5000,3,5000\n"
                                            "KL,H,5000,1000\n";

/**
 * Records of two print intervals: one tap and one scan in the first, then only one
 * scan in the second, which is 1 ms longer
 */
static const char two_interval_text[] = "KL,P,5000,record_user,2,80,40\n"
                                        "KL,P,5000,key_log,2,80,40\n"
                                        "KL,P,5000,plain_mode,2,80,40\n"
                                        "KL,P,5000,case_modes,2,80,40\n"
                                        "KL,P,5000,bracket_pairs,2,80,40\n"
                                        "KL,P,5000,special_mode,2,80,40\n"
                                        "KL,P,5000,symbol_holds,2,80,40\n"
                                        "KL,P,5000,precision_key,2,80,40\n"
                                        "KL,R,5000,1,5000\n"
                                        "KL,H,5000,1000\n"
                                        "KL,P,10001,record_user,0,0,0\n"
                                        "KL,P,10001,key_log,0,0,0\n"
                                        "KL,P,10001,plain_mode,0,0,0\n"
                                        "KL,P,10001,case_modes,0,0,0\n"
                                        "KL,P,10001,bracket_pairs,0,0,0\n"
                                        "KL,P,10001,special_mode,0,0,0\n"
                                        "KL,P,10001,symbol_holds,0,0,0\n"
                                        "KL,P,10001,precision_key,0,0,0\n"
                                        "KL,R,10001,1,5001\n"
                                        "KL,H,10001,1000\n";

/**
 * Sends a push or a release of a key to process_record_user().
 *
 * @param keycode Keycode after layer resolution.
 * @param pressed True for a push.
 * @return Result of process_record_user().
 */
static bool send_key_event(uint16_t keycode, bool pressed) {
    keyrecord_t record = {.event = {.pressed = pressed}};
    return process_record_user(keycode, &record);
}

/**
 * Pushes and releases a key.
 *
 * @param keycode Keycode after layer resolution.
 */
static void tap_key(uint16_t keycode) {
    send_key_event(keycode, true);
    send_key_event(keycode, false);
}

/**
 * Sets profiling on or off at time 0, then empties the console text and the call
 * log. The print interval starts at time 0.
 *
 * @param on True for profiling on.
 */
static void start_with_profiling(bool on) {
    reset_fake_qmk();
    reset_fake_profiling_hardware();
    case_modes_end_all();
    if (profiling_is_on()) {
        tap_key(PROFILING_TOGGLE);
    }
    if (on) {
        tap_key(PROFILING_TOGGLE);
    }
    reset_fake_qmk();
    set_fake_call_cycles(CALL_CYCLES);
    set_fake_free_stack_bytes(FREE_STACK_BYTES);
}

/** Moves the clock to the end of the first print interval, then scans once */
static void scan_at_interval_end(void) {
    advance_fake_clock(PROFILING_INTERVAL_MILLISECONDS);
    matrix_scan_user();
}

static void profiling_toggle_key_is_at_left_bottom_row_column_0(void **state) {
    (void)state;
    uint16_t keycode =
        keycode_at_keymap_location(LAYER_BASE, LEFT_BOTTOM_ROW, TOGGLE_KEY_COLUMN);
    assert_int_equal(keycode, PROFILING_TOGGLE);
}

static void profiling_off_writes_no_records(void **state) {
    (void)state;
    start_with_profiling(false);
    tap_key(KC_A);
    matrix_scan_user();
    scan_at_interval_end();
    assert_console_text("");
}

static void profiling_toggle_writes_clock_record(void **state) {
    (void)state;
    start_with_profiling(false);
    advance_fake_clock(TOGGLE_TIME);
    tap_key(PROFILING_TOGGLE);
    assert_true(profiling_is_on());
    assert_console_text("KL,C,1234,72000000\n");
}

static void profiling_toggle_sends_no_key(void **state) {
    (void)state;
    start_with_profiling(false);
    assert_false(send_key_event(PROFILING_TOGGLE, true));
    assert_false(send_key_event(PROFILING_TOGGLE, false));
    assert_recorded_calls(NULL, 0);
}

static void profiling_toggle_starts_cycle_counter(void **state) {
    (void)state;
    start_with_profiling(false);
    tap_key(PROFILING_TOGGLE);
    assert_true(fake_cycle_counter_is_on());
}

static void profiling_toggle_off_ends_records(void **state) {
    (void)state;
    start_with_profiling(true);
    tap_key(PROFILING_TOGGLE);
    assert_false(profiling_is_on());
    tap_key(KC_A);
    scan_at_interval_end();
    assert_console_text("");
}

static void profiling_no_records_before_interval_end(void **state) {
    (void)state;
    start_with_profiling(true);
    tap_key(KC_A);
    advance_fake_clock(PROFILING_INTERVAL_MILLISECONDS - ONE_MILLISECOND);
    matrix_scan_user();
    assert_console_text("");
}

static void profiling_interval_writes_records_of_each_call(void **state) {
    (void)state;
    start_with_profiling(true);
    tap_key(KC_A);
    matrix_scan_user();
    matrix_scan_user();
    scan_at_interval_end();
    assert_console_text(one_tap_interval_text);
}

static void profiling_records_are_for_one_interval(void **state) {
    (void)state;
    start_with_profiling(true);
    tap_key(KC_A);
    scan_at_interval_end();
    advance_fake_clock(PROFILING_INTERVAL_MILLISECONDS + ONE_MILLISECOND);
    matrix_scan_user();
    assert_console_text(two_interval_text);
}

static void profiling_handled_event_skips_later_handlers(void **state) {
    (void)state;
    start_with_profiling(true);
    assert_false(send_key_event(SNAKE_CASE_KEY, true));
    scan_at_interval_end();
    case_modes_end_all();
    assert_console_text("KL,P,5000,record_user,1,40,40\n"
                        "KL,P,5000,key_log,1,40,40\n"
                        "KL,P,5000,plain_mode,1,40,40\n"
                        "KL,P,5000,case_modes,1,40,40\n"
                        "KL,P,5000,bracket_pairs,0,0,0\n"
                        "KL,P,5000,special_mode,0,0,0\n"
                        "KL,P,5000,symbol_holds,0,0,0\n"
                        "KL,P,5000,precision_key,0,0,0\n"
                        "KL,R,5000,1,5000\n"
                        "KL,H,5000,1000\n");
}

static void profiling_plain_mode_push_skips_normal_handlers(void **state) {
    (void)state;
    start_with_profiling(true);
    layer_state = ONLY_LAYER(LAYER_PLAIN);
    assert_true(send_key_event(KC_A, true));
    scan_at_interval_end();
    assert_console_text("KL,P,5000,record_user,1,40,40\n"
                        "KL,P,5000,key_log,1,40,40\n"
                        "KL,P,5000,plain_mode,1,40,40\n"
                        "KL,P,5000,case_modes,0,0,0\n"
                        "KL,P,5000,bracket_pairs,0,0,0\n"
                        "KL,P,5000,special_mode,0,0,0\n"
                        "KL,P,5000,symbol_holds,0,0,0\n"
                        "KL,P,5000,precision_key,0,0,0\n"
                        "KL,R,5000,1,5000\n"
                        "KL,H,5000,1000\n");
}

static void profiling_toggle_on_again_clears_statistics(void **state) {
    (void)state;
    start_with_profiling(true);
    tap_key(KC_A);
    matrix_scan_user();
    tap_key(PROFILING_TOGGLE);
    tap_key(PROFILING_TOGGLE);
    reset_fake_qmk();
    scan_at_interval_end();
    assert_console_text("KL,P,5000,record_user,0,0,0\n"
                        "KL,P,5000,key_log,0,0,0\n"
                        "KL,P,5000,plain_mode,0,0,0\n"
                        "KL,P,5000,case_modes,0,0,0\n"
                        "KL,P,5000,bracket_pairs,0,0,0\n"
                        "KL,P,5000,special_mode,0,0,0\n"
                        "KL,P,5000,symbol_holds,0,0,0\n"
                        "KL,P,5000,precision_key,0,0,0\n"
                        "KL,R,5000,1,5000\n"
                        "KL,H,5000,1000\n");
}

/** Cases of this program, in run order */
static const struct CMUnitTest profiling_cases[] = {
    cmocka_unit_test(profiling_toggle_key_is_at_left_bottom_row_column_0),
    cmocka_unit_test(profiling_off_writes_no_records),
    cmocka_unit_test(profiling_toggle_writes_clock_record),
    cmocka_unit_test(profiling_toggle_sends_no_key),
    cmocka_unit_test(profiling_toggle_starts_cycle_counter),
    cmocka_unit_test(profiling_toggle_off_ends_records),
    cmocka_unit_test(profiling_no_records_before_interval_end),
    cmocka_unit_test(profiling_interval_writes_records_of_each_call),
    cmocka_unit_test(profiling_records_are_for_one_interval),
    cmocka_unit_test(profiling_handled_event_skips_later_handlers),
    cmocka_unit_test(profiling_plain_mode_push_skips_normal_handlers),
    cmocka_unit_test(profiling_toggle_on_again_clears_statistics),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(profiling_cases, NULL, NULL);
}
