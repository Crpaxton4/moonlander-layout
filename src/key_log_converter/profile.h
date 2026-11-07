/** @file
 * Profiling totals of the record handlers, the matrix scans and the stack
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "counters.h"
#include "record.h"
#include "support.h"

// #endregion

// #region Macros

/** Core clock of a run or a file with no clock record */
#define NO_CLOCK 0

// #endregion

// #region Types

/**
 * Totals of one handler. Each total is a sum or a maximum of whole numbers, so the
 * totals of all intervals, sessions and files are exact
 */
typedef struct {
    char     name[HANDLER_NAME_SIZE]; /**< Handler name */
    count_t  calls;                   /**< Calls */
    count_t  cycle_sum;               /**< CPU cycles of all calls */
    count_t  cycle_max;               /**< CPU cycles of the slowest call */
    size_t   worst_session;           /**< Session of the first record with cycle_max */
    uint32_t worst_time;              /**< Time of the first record with cycle_max */
} handler_totals_t;

/** Handler totals in the order of their first handler record */
typedef struct {
    handler_totals_t *items;    /**< Totals, or NULL before the first record */
    size_t            count;    /**< Items in use */
    size_t            capacity; /**< Items that the array can hold */
} handler_table_t;

/** Matrix scans of a group of intervals */
typedef struct {
    count_t scan_count;  /**< Matrix scans */
    count_t interval_ms; /**< Length of all intervals in milliseconds */
} scan_totals_t;

/** Profiling totals of a run */
typedef struct {
    count_t         records;               /**< Profiling records in sessions */
    count_t         records_without_clock; /**< Handler records before a clock */
    uint32_t        cpu_hz;                /**< Last clock of the run, or NO_CLOCK */
    handler_table_t handlers;              /**< Totals of each handler */
    scan_totals_t   idle_scans;            /**< Scans of intervals with no input */
    scan_totals_t   typing_scans;          /**< Scans of intervals with input */
    bool            has_stack;             /**< true after a stack record */
    uint32_t        lowest_free_stack;     /**< Lowest free stack of all records */
} profile_t;

// #endregion

// #region Functions

/**
 * Adds a handler record to the totals of its handler.
 *
 * @param profile Profiling totals.
 * @param record  Handler record.
 * @param session Number of the session of the record, from 1.
 */
void add_handler_interval(profile_t *profile, const record_t *record, size_t session);

/**
 * Adds the scans of a scan record.
 *
 * @param profile Profiling totals.
 * @param scans   Values of the scan record.
 * @param typing  true if the interval has an input record.
 */
void add_scan_interval(profile_t *profile, const scan_interval_t *scans, bool typing);

/**
 * Adds a stack record.
 *
 * @param profile    Profiling totals.
 * @param free_bytes Lowest free stack of the record.
 */
void add_stack_record(profile_t *profile, uint32_t free_bytes);

/**
 * Gives the scans of all intervals, idle and typing.
 *
 * @param profile Profiling totals.
 * @return Scans of all intervals.
 */
scan_totals_t all_scans(const profile_t *profile);

/**
 * Gives the average time of one matrix scan of all intervals.
 *
 * @param profile Profiling totals.
 * @return Seconds as a ratio, with a whole of 0 if no interval has a scan.
 */
ratio_t average_scan_time(const profile_t *profile);

/**
 * Gives the average time of one call of a handler. The clock of the run applies
 * to all handler records, so the ratio uses the exact cycle sum.
 *
 * @param profile Profiling totals with the clock of the run.
 * @param handler Handler totals.
 * @return Seconds as a ratio, with a whole of 0 for 0 calls or no clock.
 */
ratio_t average_call_time(const profile_t *profile, const handler_totals_t *handler);

/**
 * Gives the time of the slowest call of a handler.
 *
 * @param profile Profiling totals with the clock of the run.
 * @param handler Handler totals.
 * @return Seconds as a ratio, with a whole of 0 for no clock.
 */
ratio_t slowest_call_time(const profile_t *profile, const handler_totals_t *handler);

// #endregion
