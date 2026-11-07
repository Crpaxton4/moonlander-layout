/** @file
 * Hardware access of the profiling debug build: the core cycle counter, the core
 * clock and the main stack.
 *
 * Unit tests replace profiling_hardware.c with a fake, because the host has no
 * cycle counter and no ChibiOS stack.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "profiling/profiling.h"

/** Starts the core cycle counter, if it is not on */
void start_cycle_counter(void);

/**
 * Gives a key event to a handler and counts the core cycles of that call.
 *
 * @param handler Key event handler.
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @param cycles  Gets the core cycles of the call.
 * @return Result of the handler.
 */
bool measure_handler_cycles(measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record, uint32_t *cycles);

/**
 * Counts the bytes of the main stack that no function used since power-up.
 *
 * @return Free bytes of the main stack.
 */
uint32_t main_stack_free_bytes(void);

/**
 * Gives the core clock.
 *
 * @return Core clock in Hz.
 */
uint32_t core_clock_hz(void);
