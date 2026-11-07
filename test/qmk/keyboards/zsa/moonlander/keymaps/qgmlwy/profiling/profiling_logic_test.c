/** @file
 * Tests of the profiling statistics, the print interval and the free stack count,
 * with no QMK code.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "profiling/profiling_logic.h"
#include "unit_test.h"

/** Time in ms at which each case switches profiling on */
#define START_TIME 1000

/** Shortest time step */
#define ONE_MILLISECOND 1

/** Time after two print intervals */
#define TWO_INTERVALS (2 * PROFILING_INTERVAL_MILLISECONDS)

/** Time in ms after the interval end at which a late check comes */
#define LATE_CHECK_DELAY 3

/** Cycles of a short call */
#define SHORT_CALL_CYCLES 40

/** Cycles of a call between the short and the long one */
#define MIDDLE_CALL_CYCLES 100

/** Cycles of a long call */
#define LONG_CALL_CYCLES 250

/** Cycles of a call that an interrupt or a console write made slow */
#define VERY_LONG_CALL_CYCLES 9000

/** Bytes in one stack word */
#define WORD_BYTES 4

/** Stack word that a function wrote, so it is not the fill word */
#define USED_STACK_WORD UINT32_C(0x20001234)

/** Number of elements of an array */
#define ELEMENT_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/**
 * Sets a profile to its power-up state, then switches profiling on.
 *
 * @param profile Profile to start.
 */
static void start_profile(profile_t *profile) {
    memset(profile, 0, sizeof(*profile));
    assert_true(profile_toggle(profile, START_TIME));
}

/**
 * Fails the test if a statistic of a measured call or the scan count is not zero.
 *
 * @param profile Profile to check.
 */
static void assert_statistics_are_zero(const profile_t *profile) {
    for (size_t call = 0; call < PROFILED_CALL_COUNT; call++) {
        const call_statistics_t *statistics = &profile->statistics[call];
        assert_int_equal(statistics->call_count, 0);
        assert_int_equal(statistics->cycle_sum, 0);
        assert_int_equal(statistics->cycle_max, 0);
    }
    assert_int_equal(profile->scan_count, 0);
}

/**
 * Adds a call to each measured call and one matrix scan.
 *
 * @param profile Profile to change.
 */
static void add_one_of_each(profile_t *profile) {
    for (size_t call = 0; call < PROFILED_CALL_COUNT; call++) {
        call_statistics_add(&profile->statistics[call], SHORT_CALL_CYCLES);
    }
    profile_note_scan(profile);
}

static void profiling_logic_power_up_state_is_off(void **state) {
    (void)state;
    profile_t profile;
    memset(&profile, 0, sizeof(profile));
    assert_false(profile.is_on);
    assert_false(profile_interval_ended(&profile, START_TIME + TWO_INTERVALS));
}

static void profiling_logic_toggle_switches_on_and_off(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    assert_true(profile.is_on);
    assert_false(profile_toggle(&profile, START_TIME));
    assert_false(profile.is_on);
    assert_true(profile_toggle(&profile, START_TIME));
    assert_true(profile.is_on);
}

static void profiling_logic_start_resets_statistics(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    add_one_of_each(&profile);
    assert_false(profile_toggle(&profile, START_TIME));
    uint32_t restart_time = START_TIME + ONE_MILLISECOND;
    assert_true(profile_toggle(&profile, restart_time));
    assert_statistics_are_zero(&profile);
    assert_int_equal(profile.interval_start, restart_time);
}

static void profiling_logic_calls_count_and_sum_the_cycles(void **state) {
    (void)state;
    call_statistics_t statistics = {0};
    call_statistics_add(&statistics, MIDDLE_CALL_CYCLES);
    call_statistics_add(&statistics, LONG_CALL_CYCLES);
    call_statistics_add(&statistics, SHORT_CALL_CYCLES);
    assert_int_equal(statistics.call_count, 3);
    assert_int_equal(statistics.cycle_sum, 390);
    assert_int_equal(statistics.cycle_max, 250);
}

static void profiling_logic_maximum_is_the_slowest_call(void **state) {
    (void)state;
    call_statistics_t statistics = {0};
    call_statistics_add(&statistics, SHORT_CALL_CYCLES);
    call_statistics_add(&statistics, VERY_LONG_CALL_CYCLES);
    call_statistics_add(&statistics, MIDDLE_CALL_CYCLES);
    assert_int_equal(statistics.cycle_max, 9000);
}

static void profiling_logic_scans_count(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    profile_note_scan(&profile);
    profile_note_scan(&profile);
    profile_note_scan(&profile);
    assert_int_equal(profile.scan_count, 3);
}

static void profiling_logic_interval_ends_after_5000_ms(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    uint32_t interval_end = START_TIME + PROFILING_INTERVAL_MILLISECONDS;
    assert_int_equal(PROFILING_INTERVAL_MILLISECONDS, 5000);
    assert_false(profile_interval_ended(&profile, interval_end - ONE_MILLISECOND));
    assert_true(profile_interval_ended(&profile, interval_end));
}

static void profiling_logic_interval_length_is_the_time_since_its_start(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    uint32_t late_check =
        START_TIME + PROFILING_INTERVAL_MILLISECONDS + LATE_CHECK_DELAY;
    assert_int_equal(profile_interval_length(&profile, late_check), 5003);
}

static void profiling_logic_interval_ends_when_the_timer_wraps(void **state) {
    (void)state;
    profile_t profile;
    memset(&profile, 0, sizeof(profile));
    uint32_t before_wrap = UINT32_MAX - ONE_MILLISECOND;
    assert_true(profile_toggle(&profile, before_wrap));
    uint32_t after_wrap = before_wrap + PROFILING_INTERVAL_MILLISECONDS;
    assert_true(profile_interval_ended(&profile, after_wrap));
    assert_int_equal(profile_interval_length(&profile, after_wrap), 5000);
}

static void profiling_logic_new_interval_resets_statistics(void **state) {
    (void)state;
    profile_t profile;
    start_profile(&profile);
    add_one_of_each(&profile);
    uint32_t interval_end = START_TIME + PROFILING_INTERVAL_MILLISECONDS;
    profile_start_interval(&profile, interval_end);
    assert_statistics_are_zero(&profile);
    assert_int_equal(profile.interval_start, interval_end);
    assert_true(profile.is_on);
    assert_false(profile_interval_ended(&profile, interval_end));
}

static void profiling_logic_call_names_follow_the_record_contract(void **state) {
    (void)state;
    static const char *const expected_names[] = {
        [PROFILED_RECORD_USER]   = "record_user",
        [PROFILED_KEY_LOG]       = "key_log",
        [PROFILED_PLAIN_MODE]    = "plain_mode",
        [PROFILED_CASE_MODES]    = "case_modes",
        [PROFILED_BRACKET_PAIRS] = "bracket_pairs",
        [PROFILED_SPECIAL_MODE]  = "special_mode",
        [PROFILED_SYMBOL_HOLDS]  = "symbol_holds",
        [PROFILED_PRECISION_KEY] = "precision_key",
    };
    assert_int_equal(ELEMENT_COUNT(expected_names), PROFILED_CALL_COUNT);
    assert_int_equal(FIRST_PROFILED_HANDLER, PROFILED_KEY_LOG);
    for (size_t call = 0; call < PROFILED_CALL_COUNT; call++) {
        assert_string_equal(profiled_call_name(call), expected_names[call]);
    }
}

static void profiling_logic_free_stack_ends_at_the_first_used_word(void **state) {
    (void)state;
    static const uint32_t stack[] = {
        STACK_FILL_WORD, STACK_FILL_WORD, STACK_FILL_WORD,
        USED_STACK_WORD, STACK_FILL_WORD, USED_STACK_WORD,
    };
    assert_int_equal(free_stack_bytes(stack, ELEMENT_COUNT(stack)), 3 * WORD_BYTES);
}

static void profiling_logic_unused_stack_is_all_free(void **state) {
    (void)state;
    static const uint32_t stack[] = {STACK_FILL_WORD, STACK_FILL_WORD};
    assert_int_equal(free_stack_bytes(stack, ELEMENT_COUNT(stack)), 2 * WORD_BYTES);
}

static void profiling_logic_full_stack_has_no_free_bytes(void **state) {
    (void)state;
    static const uint32_t stack[] = {USED_STACK_WORD, STACK_FILL_WORD};
    assert_int_equal(free_stack_bytes(stack, ELEMENT_COUNT(stack)), 0);
}

static void profiling_logic_fill_word_is_the_chibios_pattern(void **state) {
    (void)state;
    assert_int_equal(STACK_FILL_WORD, 0x55555555);
}

/** Cases of this program, in run order */
static const struct CMUnitTest profiling_logic_cases[] = {
    cmocka_unit_test(profiling_logic_power_up_state_is_off),
    cmocka_unit_test(profiling_logic_toggle_switches_on_and_off),
    cmocka_unit_test(profiling_logic_start_resets_statistics),
    cmocka_unit_test(profiling_logic_calls_count_and_sum_the_cycles),
    cmocka_unit_test(profiling_logic_maximum_is_the_slowest_call),
    cmocka_unit_test(profiling_logic_scans_count),
    cmocka_unit_test(profiling_logic_interval_ends_after_5000_ms),
    cmocka_unit_test(profiling_logic_interval_length_is_the_time_since_its_start),
    cmocka_unit_test(profiling_logic_interval_ends_when_the_timer_wraps),
    cmocka_unit_test(profiling_logic_new_interval_resets_statistics),
    cmocka_unit_test(profiling_logic_call_names_follow_the_record_contract),
    cmocka_unit_test(profiling_logic_free_stack_ends_at_the_first_used_word),
    cmocka_unit_test(profiling_logic_unused_stack_is_all_free),
    cmocka_unit_test(profiling_logic_full_stack_has_no_free_bytes),
    cmocka_unit_test(profiling_logic_fill_word_is_the_chibios_pattern),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(profiling_logic_cases, NULL, NULL);
}
