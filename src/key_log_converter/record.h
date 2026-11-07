/** @file
 * Records of the keyboard key event log and the parser of one log line
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/** Key code slots of a keyboard report */
#define REPORT_KEY_COUNT 6

/** Key code of an unused report slot */
#define NO_KEY 0

/** Bytes of the longest handler name of a handler record, with its terminator */
#define HANDLER_NAME_SIZE 32

/** Record types, from the second field of a record */
typedef enum {
    RECORD_STATE,      /**< Logging changed to on or off */
    RECORD_INPUT,      /**< Key push or release */
    RECORD_OUTPUT,     /**< Keyboard report to the host */
    RECORD_CLOCK,      /**< Profiling changed to on, with the core clock */
    RECORD_HANDLER,    /**< Cycle counts of one handler in one profiling interval */
    RECORD_SCANS,      /**< Matrix scans in one profiling interval */
    RECORD_STACK,      /**< Lowest free main stack since power-up */
    RECORD_TYPE_COUNT, /**< Number of record types */
} record_type_t;

/** Logging states of a state record */
typedef enum {
    LOGGING_ON,       /**< Operator switched logging on */
    LOGGING_OFF,      /**< Operator switched logging off */
    LOGGING_IDLE_OFF, /**< Keyboard switched logging off after an idle time */
} logging_state_t;

/** Key position in QMK matrix coordinates */
typedef struct {
    uint8_t row; /**< Matrix row */
    uint8_t col; /**< Matrix column */
} position_t;

/** Fields of an input record that the outputs use */
typedef struct {
    bool       pushed;   /**< true for a push, false for a release */
    position_t position; /**< Key position */
    uint8_t    layer;    /**< Highest active layer */
    uint16_t   keycode;  /**< QMK keycode after layer resolution */
} key_event_t;

/** Keyboard report of an output record */
typedef struct {
    uint8_t mods;                   /**< HID modifier byte */
    uint8_t keys[REPORT_KEY_COUNT]; /**< HID key codes, NO_KEY in unused slots */
} report_t;

/** Cycle counts of one handler in one profiling interval. No value is an average */
typedef struct {
    char     name[HANDLER_NAME_SIZE]; /**< Handler name */
    uint32_t calls;                   /**< Calls in the interval */
    uint32_t cycle_sum;               /**< CPU cycles of all calls in the interval */
    uint32_t cycle_max;               /**< CPU cycles of the slowest call */
} handler_interval_t;

/** Matrix scans of one profiling interval */
typedef struct {
    uint32_t scan_count;  /**< Matrix scans in the interval */
    uint32_t interval_ms; /**< Length of the interval in milliseconds, not 0 */
} scan_interval_t;

/** One complete log record. Only the members of its type are valid */
typedef struct {
    record_type_t      type;    /**< Record type */
    uint32_t           time;    /**< Milliseconds since the keyboard got power */
    logging_state_t    state;   /**< New logging state of a state record */
    key_event_t        event;   /**< Key event of an input record */
    report_t           report;  /**< Report of an output record */
    uint32_t           cpu_hz;  /**< Core clock of a clock record, not 0 */
    handler_interval_t handler; /**< Values of a handler record */
    scan_interval_t    scans;   /**< Values of a scan record */
    uint32_t           stack_free_bytes; /**< Lowest free stack of a stack record */
} record_t;

/**
 * Parses one log line. A line that is not a complete record gives false.
 *
 * @param line   Line with no line break. The parser changes its commas.
 * @param record Record that gets the values.
 * @return true for a complete record.
 */
bool parse_record(char *line, record_t *record);
