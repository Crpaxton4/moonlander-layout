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

/** Record types, from the second field of a record */
typedef enum {
    RECORD_STATE,      /**< Logging changed to on or off */
    RECORD_INPUT,      /**< Key push or release */
    RECORD_OUTPUT,     /**< Keyboard report to the host */
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

/** One complete log record. Only the members of its type are valid */
typedef struct {
    record_type_t   type;   /**< Record type */
    uint32_t        time;   /**< Milliseconds since the keyboard got power */
    logging_state_t state;  /**< New logging state of a state record */
    key_event_t     event;  /**< Key event of an input record */
    report_t        report; /**< Report of an output record */
} record_t;

/**
 * Parses one log line. A line that is not a complete record gives false.
 *
 * @param line   Line with no line break. The parser changes its commas.
 * @param record Record that gets the values.
 * @return true for a complete record.
 */
bool parse_record(char *line, record_t *record);
