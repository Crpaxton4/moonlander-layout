/** @file
 * Key event log of the qgmlwy keymap: records of the key events and of the reports
 * to the host, written on the QMK console while logging is on.
 *
 * Each record is one line that starts with `KL,`. A state record gives the logging
 * state, an input record a key event, and an output record a changed keyboard
 * report.
 */

#pragma once

#include QMK_KEYBOARD_H

#include "key_event_log/key_event_log_logic.h"

/**
 * Writes an input record of a key event while logging is on, and switches logging
 * on or off for the logging combo.
 *
 * Runs first in process_record_user(), so it sees each key event.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event.
 * @return False for the logging combo, which sends no key; true otherwise.
 */
bool process_key_event_log(uint16_t keycode, keyrecord_t *record);

/**
 * Switches logging off after the idle time, and puts the logging host driver in
 * place.
 *
 * QMK sets its host driver after keyboard_post_init_user(), so this check runs on
 * each housekeeping pass.
 */
void key_event_log_housekeeping(void);

/**
 * Tells if logging is on.
 *
 * @return True while logging is on.
 */
bool key_event_log_is_on(void);
