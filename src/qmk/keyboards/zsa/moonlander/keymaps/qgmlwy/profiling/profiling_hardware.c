/** @file
 * Cycle counter, core clock and main stack of the Moonlander's STM32F303, a
 * Cortex-M4 core with ChibiOS.
 *
 * ChibiOS's startup code fills the main thread stack with STACK_FILL_WORD before
 * main(). QMK's main loop runs on that stack, and interrupts use the exception
 * stack. Thus the free bytes of the main thread stack show the deepest call of the
 * keymap code since power-up.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "profiling/profiling_hardware.h"
#include "profiling/profiling_logic.h"

// #endregion

// #region Data

// Linker script of ChibiOS gives these names, which C reserves for the toolchain
// NOLINTBEGIN(bugprone-reserved-identifier,readability-identifier-naming)

/** Lowest word of the main thread stack */
extern uint32_t __main_thread_stack_base__[];

/** Word after the highest word of the main thread stack */
extern uint32_t __main_thread_stack_end__[];

// NOLINTEND(bugprone-reserved-identifier,readability-identifier-naming)

// #endregion

// #region Public functions

void start_cycle_counter(void) {
    // ChibiOS starts the counter at boot, but profiling must not depend on the
    // port. The count does not go to zero, because ChibiOS's delays read it
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

bool measure_handler_cycles(measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record, uint32_t *cycles) {
    // Each count also has the probe cost, from the call, the return and the two
    // reads: 7 to 15 cycles for an empty handler. The flash wait states after each
    // branch give the range
    uint32_t start  = DWT->CYCCNT;
    bool     result = handler(keycode, record);
    // Unsigned subtraction stays correct when the counter wraps
    *cycles = DWT->CYCCNT - start;
    return result;
}

uint32_t main_stack_free_bytes(void) {
    // C defines a pointer subtraction only in one array, and the two linker symbols
    // are two objects for the compiler
    uintptr_t stack_bytes =
        (uintptr_t)__main_thread_stack_end__ - (uintptr_t)__main_thread_stack_base__;
    return free_stack_bytes(__main_thread_stack_base__,
                            stack_bytes / sizeof(*__main_thread_stack_base__));
}

uint32_t core_clock_hz(void) {
    return STM32_SYSCLK;
}

// #endregion
