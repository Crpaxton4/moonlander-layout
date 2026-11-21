/** @file
 * Logging pause, idle time and report comparison of the key event log, with no
 * QMK code.
 *
 * key_event_log.c gives these functions the QMK values and writes the records that
 * they select.
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// #endregion

// #region Macros

/** Key slots of an output record, as in a 6-key keyboard report */
#define LOGGED_REPORT_KEY_COUNT 6

/** Time with no key push that ends a pause, so a pause cannot stay by mistake */
#define PAUSE_IDLE_MILLISECONDS (UINT32_C(5) * 60 * 1000)

// #endregion

// #region Enumerations

/** Value of a state record */
enum logging_state {
    LOGGING_OFF,     /**< Operator paused logging */
    LOGGING_ON,      /**< Operator ended the pause */
    LOGGING_IDLE_ON, /**< Idle time ended the pause */
};

// #endregion

// #region Types

/** Modifiers and keys of one keyboard report, as an output record gives them */
typedef struct {
    uint8_t modifiers;                     /**< Modifier byte */
    uint8_t keys[LOGGED_REPORT_KEY_COUNT]; /**< HID key codes, 0 for an unused slot */
} logged_report_t;

/** State of the key event log; all zero is the state at power-up, logging on */
typedef struct {
    bool            is_paused;      /**< Logging has a pause */
    uint32_t        last_push_time; /**< Time of the last key push, in ms */
    logged_report_t last_report;    /**< Last report that the keyboard sent */
} logging_t;

// #endregion

// #region Functions

/**
 * Pauses logging, or ends the pause. A pause starts the idle time.
 *
 * @param logging Logging state.
 * @param now     Time in ms.
 * @return LOGGING_OFF for a new pause, LOGGING_ON for the end of a pause.
 */
enum logging_state logging_toggle(logging_t *logging, uint32_t now);

/**
 * Starts the idle time again, because the operator pushed a key.
 *
 * @param logging Logging state.
 * @param now     Time in ms.
 */
void logging_note_push(logging_t *logging, uint32_t now);

/**
 * Ends a pause if the idle time is over.
 *
 * @param logging Logging state.
 * @param now     Time in ms; the subtraction stays correct when the timer wraps.
 * @return True if the pause ended now.
 */
bool logging_pause_ended(logging_t *logging, uint32_t now);

/**
 * Tells if a report is different from the last report, then keeps it as the last
 * report.
 *
 * @param logging Logging state with the last report.
 * @param report  Report that the keyboard sends now.
 * @return True if the modifiers or a key slot changed.
 */
bool logging_report_changed(logging_t *logging, const logged_report_t *report);

/**
 * Fills the key slots of an output record from the key bitmap of an NKRO report.
 *
 * Bit n of the bitmap is HID key code n. The slots get the first six keys in key
 * code order, because an output record has six key slots.
 *
 * @param keys       LOGGED_REPORT_KEY_COUNT key slots to fill, 0 for an unused slot.
 * @param bits       Key bitmap.
 * @param byte_count Bytes in the bitmap.
 */
void keys_from_bitmap(uint8_t *keys, const uint8_t *bits, size_t byte_count);

/**
 * Gives the text of a state for a state record.
 *
 * @param state State.
 * @return "off", "on" or "idle-on".
 */
const char *logging_state_name(enum logging_state state);

/**
 * Gives the direction letter of an input record.
 *
 * @param pressed True for a push, false for a release.
 * @return 'd' for a push, 'u' for a release.
 */
char key_event_direction(bool pressed);

// #endregion
