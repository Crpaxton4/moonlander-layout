/** @file
 * Host version of the profiling hardware access, with the values that a test sets.
 */

#include "profiling/profiling_hardware.h"

#include "fake_profiling_hardware.h"

/** Cycles of each measured call */
static uint32_t call_cycles;

/** Free bytes of the main stack */
static uint32_t free_stack;

/** True after start_cycle_counter() */
static bool cycle_counter_is_on;

void reset_fake_profiling_hardware(void) {
    call_cycles         = 0;
    free_stack          = 0;
    cycle_counter_is_on = false;
}

void set_fake_call_cycles(uint32_t cycles) {
    call_cycles = cycles;
}

void set_fake_free_stack_bytes(uint32_t bytes) {
    free_stack = bytes;
}

bool fake_cycle_counter_is_on(void) {
    return cycle_counter_is_on;
}

void start_cycle_counter(void) {
    cycle_counter_is_on = true;
}

bool measure_handler_cycles(measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record, uint32_t *cycles) {
    *cycles = call_cycles;
    return handler(keycode, record);
}

uint32_t main_stack_free_bytes(void) {
    return free_stack;
}

uint32_t core_clock_hz(void) {
    return FAKE_CORE_CLOCK_HZ;
}
