/** @file
 * Session state and outputs of the conversion of log records
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "counters.h"
#include "profile.h"
#include "record.h"

/** Counts that the converter writes to standard output */
typedef struct {
    size_t files;                    /**< Log files that the run read */
    size_t sessions;                 /**< Sessions in all files */
    size_t records;                  /**< Complete records in all files */
    size_t records_outside_sessions; /**< Input and output records with logging off */
    size_t ignored_lines;            /**< Lines that are not complete records */
} summary_t;

/** State of one log file. Each file starts with no session and no clock */
typedef struct {
    bool     in_session;      /**< true between a state record on and off */
    bool     has_time;        /**< false before the first record */
    uint32_t time;            /**< Time of the record before */
    report_t previous;        /**< Report of the output record before */
    uint32_t session_start;   /**< Time of the first record of the session */
    bool     has_clock;       /**< true after a clock record, until a restart */
    bool     has_input;       /**< false before the first input record */
    uint32_t last_input_time; /**< Time of the last input record in a session */
} stream_t;

/** Outputs and totals of a run */
typedef struct {
    FILE             *corpus;            /**< Corpus text file */
    FILE             *heatmap;           /**< Heatmap lines file */
    stream_t          stream;            /**< State of the current log file */
    bool              corpus_line_open;  /**< Corpus line has text and no line break */
    uint8_t           highest_layer;     /**< Highest layer of all pushes */
    count_t           correction_pushes; /**< New Backspace and Delete keys */
    count_t           character_pushes;  /**< New keys that typed a character */
    position_counts_t usage;             /**< Pushes per position and layer */
    position_counts_t corrections;       /**< Backspace pushes after each position */
    recent_pushes_t   recent_pushes;     /**< Last push of each position */
    named_counts_t    shortcuts;         /**< Pushes of each shortcut */
    named_counts_t    keys;              /**< New keys of each key name */
    named_counts_t    heatmap_rows;      /**< Pushes of each heatmap line */
    count_t           key_pushes;        /**< Pushes of all input records */
    count_t           logged_ms;         /**< Time of all sessions */
    profile_t         profile;           /**< Profiling totals */
    summary_t         summary;           /**< Counts for standard output */
} converter_t;

/**
 * Starts a log file with no session and no record before it.
 *
 * @param converter Outputs and totals.
 */
void begin_stream(converter_t *converter);

/**
 * Uses one record of the current log file.
 *
 * @param converter Outputs and totals.
 * @param record    Complete record.
 */
void convert_record(converter_t *converter, const record_t *record);

/**
 * Ends the open session at the end of a log file.
 *
 * @param converter Outputs and totals.
 */
void end_stream(converter_t *converter);
