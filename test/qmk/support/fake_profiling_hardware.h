/** @file
 * State of the fake profiling hardware that a test sets or reads: the cycles of
 * each measured call, the free stack and the cycle counter.
 *
 * Host has no cycle counter and no ChibiOS stack, so fake_profiling_hardware.c
 * gives fixed values in place of profiling_hardware.c.
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stdint.h>

// #endregion

// #region Macros

/** Core clock that the fake gives, the clock of the Moonlander */
#define FAKE_CORE_CLOCK_HZ 72000000

// #endregion

// #region Functions

/** Sets all fake values to zero and the cycle counter to off */
void reset_fake_profiling_hardware(void);

/**
 * Sets the cycles that each measured call takes.
 *
 * @param cycles Core cycles of each call.
 */
void set_fake_call_cycles(uint32_t cycles);

/**
 * Sets the free bytes of the main stack.
 *
 * @param bytes Free bytes.
 */
void set_fake_free_stack_bytes(uint32_t bytes);

/**
 * Tells if the profiling started the cycle counter.
 *
 * @return True after start_cycle_counter().
 */
bool fake_cycle_counter_is_on(void);

// #endregion
