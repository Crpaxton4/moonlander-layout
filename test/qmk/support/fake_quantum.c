/** @file
 * Recording versions of the QMK functions that the keymap code calls, and the call
 * log and fake clock that the tests read.
 *
 * QMK declares most of these functions in its own headers, so a wrong signature here
 * does not compile.
 */

#include QMK_KEYBOARD_H

#include <stdbool.h>

#include "recorded_calls.h"
#include "unit_test.h"

/** Calls that one test case can record; a longer log fails the case */
#define CALL_LOG_CAPACITY 64

/** Calls since the last reset_fake_qmk(), in call order */
static qmk_call_t recorded_calls[CALL_LOG_CAPACITY];

/** Number of calls in recorded_calls */
static size_t recorded_call_count;

/** Time in milliseconds since the last reset_fake_qmk() */
static uint32_t fake_clock_milliseconds;

/** Name of each recorded function, for the failure output */
static const char *const function_names[] = {
    [CALLED_REGISTER_CODE]     = "register_code",
    [CALLED_UNREGISTER_CODE]   = "unregister_code",
    [CALLED_TAP_CODE]          = "tap_code",
    [CALLED_REGISTER_CODE16]   = "register_code16",
    [CALLED_UNREGISTER_CODE16] = "unregister_code16",
    [CALLED_TAP_CODE16]        = "tap_code16",
    [CALLED_LAYER_ON]          = "layer_on",
    [CALLED_LAYER_OFF]         = "layer_off",
    [CALLED_LAYER_MOVE]        = "layer_move",
    [CALLED_WAIT_MS]           = "wait_ms",
};

/**
 * Adds one call to the end of the log.
 *
 * @param function Function that the keymap code called.
 * @param argument Argument of the call.
 */
static void record_call(enum qmk_function function, uint32_t argument) {
    if (recorded_call_count == CALL_LOG_CAPACITY) {
        fail_msg("More than %d QMK calls", CALL_LOG_CAPACITY);
    }
    recorded_calls[recorded_call_count] = (qmk_call_t){
        .function = function,
        .argument = argument,
    };
    recorded_call_count++;
}

void reset_fake_qmk(void) {
    recorded_call_count     = 0;
    fake_clock_milliseconds = 0;
}

void advance_fake_clock(uint32_t milliseconds) {
    fake_clock_milliseconds += milliseconds;
}

/**
 * Prints a list of calls, one call on each line.
 *
 * @param title Name of the list.
 * @param calls Calls to print.
 * @param count Number of calls.
 */
static void print_calls(const char *title, const qmk_call_t *calls, size_t count) {
    print_message("%s: %zu calls\n", title, count);
    for (size_t index = 0; index < count; index++) {
        const char *name = function_names[calls[index].function];
        print_message("    %s(0x%04X)\n", name, (unsigned)calls[index].argument);
    }
}

/**
 * Tells if the log is the expected calls, in the same order.
 *
 * @param expected Calls that the test expects.
 * @param count    Number of expected calls.
 * @return True if each call and its argument are the same.
 */
static bool calls_match(const qmk_call_t *expected, size_t count) {
    if (count != recorded_call_count) {
        return false;
    }
    for (size_t index = 0; index < count; index++) {
        if (expected[index].function != recorded_calls[index].function ||
            expected[index].argument != recorded_calls[index].argument) {
            return false;
        }
    }
    return true;
}

void assert_recorded_calls(const qmk_call_t *expected, size_t count) {
    if (calls_match(expected, count)) {
        return;
    }
    print_calls("Expected", expected, count);
    print_calls("Recorded", recorded_calls, recorded_call_count);
    fail_msg("Recorded QMK calls are not the expected calls");
}

void register_code(uint8_t code) {
    record_call(CALLED_REGISTER_CODE, code);
}

void unregister_code(uint8_t code) {
    record_call(CALLED_UNREGISTER_CODE, code);
}

void tap_code(uint8_t code) {
    record_call(CALLED_TAP_CODE, code);
}

void register_code16(uint16_t code) {
    record_call(CALLED_REGISTER_CODE16, code);
}

void unregister_code16(uint16_t code) {
    record_call(CALLED_UNREGISTER_CODE16, code);
}

void tap_code16(uint16_t code) {
    record_call(CALLED_TAP_CODE16, code);
}

void layer_on(uint8_t layer) {
    record_call(CALLED_LAYER_ON, layer);
}

void layer_off(uint8_t layer) {
    record_call(CALLED_LAYER_OFF, layer);
}

void layer_move(uint8_t layer) {
    record_call(CALLED_LAYER_MOVE, layer);
}

void wait_ms(uint32_t milliseconds) {
    record_call(CALLED_WAIT_MS, milliseconds);
    fake_clock_milliseconds += milliseconds;
}

uint16_t timer_read(void) {
    return (uint16_t)fake_clock_milliseconds;
}

uint32_t timer_read32(void) {
    return fake_clock_milliseconds;
}

uint16_t timer_elapsed(uint16_t last) {
    return (uint16_t)(timer_read() - last);
}

uint32_t timer_elapsed32(uint32_t last) {
    return fake_clock_milliseconds - last;
}
