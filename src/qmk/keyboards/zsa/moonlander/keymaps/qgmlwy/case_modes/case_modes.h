/** @file
 * Case modes of the qgmlwy keymap: snake_case, CONSTANT_CASE, camelCase and
 * PascalCase from words that the operator types with Space between them.
 *
 * Each mode key starts its mode. The state machine in case_modes_logic.c selects
 * what each key push sends while a mode is on.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Changes, replaces or passes a key push as the case mode requires, and starts,
 * changes or ends the mode.
 *
 * Runs right after the key event log in process_record_user(), so the log sees the
 * pushed keys and no later handler sees a key that the mode replaced.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event, with its tap count.
 * @return False when the push sends other keys or no key; true otherwise, and for
 *         each release.
 */
bool process_case_modes(uint16_t keycode, keyrecord_t *record);

/** Ends any case mode, for a mode that must not get changed keys */
void case_modes_end_all(void);
