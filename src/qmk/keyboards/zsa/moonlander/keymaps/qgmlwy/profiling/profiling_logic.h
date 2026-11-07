/** @file
 * Statistics of the profiling debug build, with no QMK code: calls and cycles of
 * each measured call, matrix scans, the print interval and the free stack.
 *
 * profiling.c gives these functions the cycle counts and the time, and writes the
 * records that they select.
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// #endregion

// #region Macros

/** Time in ms between two prints of the statistics */
#define PROFILING_INTERVAL_MILLISECONDS 5000

/**
 * Word that ChibiOS's startup code writes into each stack before main(). A word
 * with another value is a word that a function used.
 */
#define STACK_FILL_WORD UINT32_C(0x55555555)

/**
 * Measured call of the first record handler. The handlers follow in the order of
 * the record handler table of keymap.c.
 */
#define FIRST_PROFILED_HANDLER PROFILED_KEY_LOG

/** Number of measured record handlers, one for each entry of the handler table */
#define PROFILED_HANDLER_COUNT (PROFILED_CALL_COUNT - FIRST_PROFILED_HANDLER)

// #endregion

// #region Enumerations

/** Measured calls, each with its own profile record */
enum profiled_call {
    PROFILED_RECORD_USER,   /**< All of process_record_user() */
    PROFILED_KEY_LOG,       /**< Record handler of the key event log */
    PROFILED_PLAIN_MODE,    /**< Record handler of the plain mode */
    PROFILED_CASE_MODES,    /**< Record handler of the case modes */
    PROFILED_BRACKET_PAIRS, /**< Record handler of the bracket pairs */
    PROFILED_SPECIAL_MODE,  /**< Record handler of the special mode */
    PROFILED_SYMBOL_HOLDS,  /**< Record handler of the symbol holds */
    PROFILED_PRECISION_KEY, /**< Record handler of the precision hold */
    PROFILED_CALL_COUNT,    /**< Number of measured calls */
};

// #endregion

// #region Types

/** Statistics of one measured call in one print interval */
typedef struct {
    uint32_t call_count; /**< Calls */
    uint32_t cycle_sum;  /**< Cycles of all calls; 5 s of a 72 MHz core fit */
    uint32_t cycle_max;  /**< Cycles of the slowest call */
} call_statistics_t;

/** State of the profiling; all zero is the state at power-up, profiling off */
typedef struct {
    bool              is_on;          /**< Profiling is on */
    uint32_t          interval_start; /**< Start time of the print interval, in ms */
    uint32_t          scan_count;     /**< Matrix scans in the print interval */
    call_statistics_t statistics[PROFILED_CALL_COUNT]; /**< Indexed by profiled_call */
} profile_t;

// #endregion

// #region Functions

/**
 * Switches profiling on or off. Switching on clears the statistics and starts a
 * print interval.
 *
 * @param profile Profile.
 * @param now     Time in ms.
 * @return True if profiling is on now.
 */
bool profile_toggle(profile_t *profile, uint32_t now);

/**
 * Adds one call to the statistics of a measured call.
 *
 * @param statistics Statistics of the measured call.
 * @param cycles     Core cycles of the call.
 */
void call_statistics_add(call_statistics_t *statistics, uint32_t cycles);

/**
 * Adds one matrix scan to the print interval.
 *
 * @param profile Profile.
 */
void profile_note_scan(profile_t *profile);

/**
 * Gives the time since the start of the print interval.
 *
 * @param profile Profile.
 * @param now     Time in ms; the subtraction stays correct when the timer wraps.
 * @return Length of the print interval in ms.
 */
uint32_t profile_interval_length(const profile_t *profile, uint32_t now);

/**
 * Tells if it is time to print the statistics.
 *
 * @param profile Profile.
 * @param now     Time in ms.
 * @return True if profiling is on and the print interval is over.
 */
bool profile_interval_ended(const profile_t *profile, uint32_t now);

/**
 * Clears the statistics and starts the next print interval, because each record
 * gives the values of one interval.
 *
 * @param profile Profile.
 * @param now     Time in ms.
 */
void profile_start_interval(profile_t *profile, uint32_t now);

/**
 * Gives the name of a measured call for its profile record.
 *
 * @param call Measured call, a profiled_call value below PROFILED_CALL_COUNT.
 * @return Name with no comma.
 */
const char *profiled_call_name(size_t call);

/**
 * Counts the free bytes of a stack that grows down. A function that used a word
 * changed its fill word, so the free bytes end at the first other word.
 *
 * @param stack_bottom Lowest word of the stack.
 * @param word_count   Words in the stack.
 * @return Bytes from the stack bottom that still hold STACK_FILL_WORD.
 */
uint32_t free_stack_bytes(const uint32_t *stack_bottom, size_t word_count);

// #endregion
