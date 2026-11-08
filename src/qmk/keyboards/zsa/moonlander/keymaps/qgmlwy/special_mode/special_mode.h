/** @file
 * Special mode of the qgmlwy keymap: LAYER_SPECIAL with digits and symbols. A tap of
 * the mode key turns the layer on for one push. A hold locks the layer on until the
 * operator ends it.
 *
 * Mode is a layer that stays on, not a momentary layer, so the operator can type a
 * full number or expression with no held key.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

// #endregion

// #region Functions

/**
 * Starts or ends the special mode for the mode key. Ends a locked mode when the
 * operator pushes the same whitespace key two times in a row. Marks the push that
 * ends a one-shot, for post_process_special_mode().
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event, with its tap count.
 * @return False for the mode key and for a double whitespace push, which send no
 *         key; true otherwise.
 */
bool process_special_mode(uint16_t keycode, keyrecord_t *record);

/**
 * Counts a push that an earlier handler took as a key that is not whitespace, so
 * that the push separates two whitespace keys and ends a one-shot.
 */
void special_mode_count_taken_push(void);

/**
 * Ends a one-shot after QMK has run the action of its push. QMK finds the action
 * and the layer for the release after process_record_user(), so both must come from
 * LAYER_SPECIAL.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 */
void post_process_special_mode(uint16_t keycode, keyrecord_t *record);

// #endregion
