/** @file
 * Recording versions of the QMK functions that the keymap code calls, and the call
 * log, fake clock and fake state that the tests read.
 *
 * QMK declares most of these functions in its own headers, so a wrong signature here
 * does not compile.
 */

#include QMK_KEYBOARD_H

#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "fake_state.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Lowest bit of an unsigned value */
#define LOWEST_BIT 1U

/** Calls that one test case can record; a longer log fails the case */
#define CALL_LOG_CAPACITY 64

/** Calls since the last reset_fake_qmk(), in call order */
static qmk_call_t recorded_calls[CALL_LOG_CAPACITY];

/** Number of calls in recorded_calls */
static size_t recorded_call_count;

/** Time in milliseconds since the last reset_fake_qmk() */
static uint32_t fake_clock_milliseconds;

/** Console text that one test case can keep; a longer text fails the case */
#define CONSOLE_CAPACITY 1024

/** Console text since the last reset_fake_qmk(), with a terminating zero */
static char console_text[CONSOLE_CAPACITY];

/** Number of characters in console_text */
static size_t console_length;

/** Active modifiers that get_mods() gives */
static uint8_t fake_modifiers;

/** Weak modifiers that get_weak_mods() gives */
static uint8_t fake_weak_modifiers;

/** One-shot modifiers that get_oneshot_mods() gives */
static uint8_t fake_one_shot_modifiers;

/** Last setting of each status LED, indexed by position from the left */
static bool status_leds[FAKE_STATUS_LED_COUNT];

layer_state_t layer_state;

layer_state_t default_layer_state;

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
    [CALLED_SEND_KEYBOARD]     = "send_keyboard",
    [CALLED_SEND_NKRO]         = "send_nkro",
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

/**
 * Records a keyboard report that the keyboard sends to the host.
 *
 * @param report Report with up to six keys.
 */
static void send_fake_keyboard(report_keyboard_t *report) {
    record_call(CALLED_SEND_KEYBOARD, report->keys[0]);
}

/**
 * Records an NKRO report that the keyboard sends to the host.
 *
 * @param report Report with a key bitmap.
 */
static void send_fake_nkro(report_nkro_t *report) {
    record_call(CALLED_SEND_NKRO, report->mods);
}

/** Host driver that records the keyboard reports, in place of the USB driver */
static host_driver_t fake_host_driver = {
    .send_keyboard = send_fake_keyboard,
    .send_nkro     = send_fake_nkro,
};

/** Driver that host_get_driver() gives */
static host_driver_t *host_driver = &fake_host_driver;

void reset_fake_qmk(void) {
    recorded_call_count     = 0;
    fake_clock_milliseconds = 0;
    console_length          = 0;
    console_text[0]         = '\0';
    fake_modifiers          = 0;
    fake_one_shot_modifiers = 0;
    fake_weak_modifiers     = 0;
    layer_state             = 0;
    default_layer_state     = 0;
    host_driver             = &fake_host_driver;
    memset(status_leds, 0, sizeof(status_leds));
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

bool layer_state_is(uint8_t layer) {
    // Same result as QMK: a layer state of 0 still has the base layer on
    if (layer_state == 0) {
        return layer == 0;
    }
    return (layer_state & ((layer_state_t)LOWEST_BIT << layer)) != 0;
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

uint8_t biton16(uint16_t bits) {
    uint8_t highest_bit = 0;
    for (size_t bit = 0; bit < sizeof(bits) * CHAR_BIT; bit++) {
        if (((bits >> bit) & LOWEST_BIT) != 0) {
            highest_bit = (uint8_t)bit;
        }
    }
    return highest_bit;
}

uint8_t get_mods(void) {
    return fake_modifiers;
}

uint8_t get_oneshot_mods(void) {
    return fake_one_shot_modifiers;
}

uint8_t get_weak_mods(void) {
    return fake_weak_modifiers;
}

void add_weak_mods(uint8_t mods) {
    fake_weak_modifiers |= mods;
}

void clear_weak_mods(void) {
    fake_weak_modifiers = 0;
}

void set_fake_modifiers(uint8_t modifiers) {
    fake_modifiers = modifiers;
}

void set_fake_one_shot_modifiers(uint8_t modifiers) {
    fake_one_shot_modifiers = modifiers;
}

void set_fake_status_led(uint8_t position, bool is_on) {
    assert_in_range(position, 0, FAKE_STATUS_LED_COUNT - 1);
    status_leds[position] = is_on;
}

bool fake_status_led_is_on(uint8_t position) {
    assert_in_range(position, 0, FAKE_STATUS_LED_COUNT - 1);
    return status_leds[position];
}

host_driver_t *host_get_driver(void) {
    return host_driver;
}

void host_set_driver(host_driver_t *driver) {
    host_driver = driver;
}

int uprintf(const char *format, ...) {
    va_list values;
    va_start(values, format);
    size_t free_space = CONSOLE_CAPACITY - console_length;
    int length = vsnprintf(console_text + console_length, free_space, format, values);
    va_end(values);
    if (length < 0 || (size_t)length >= free_space) {
        fail_msg("Console text is longer than %d characters", CONSOLE_CAPACITY - 1);
    }
    console_length += (size_t)length;
    return length;
}

void assert_console_text(const char *expected) {
    if (strcmp(console_text, expected) == 0) {
        return;
    }
    print_message("Expected console text:\n%s\n", expected);
    print_message("Console text:\n%s\n", console_text);
    fail_msg("Console text is not the expected text");
}
