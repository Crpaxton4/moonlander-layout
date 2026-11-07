/** @file
 * Bracket and quote pairs of the qgmlwy keymap: an opener sends its pair with the
 * cursor between the two characters, and Tab or the closer moves the cursor out.
 *
 * Pair stack in bracket_pairs_logic.c keeps the open pairs, so the behaviour does
 * not depend on the auto-close settings of the host program.
 */

#pragma once

// #region Includes

#include QMK_KEYBOARD_H

// #endregion

// #region Functions

/**
 * Sends pairs, jump-outs and pair deletes in place of the pushed key, and counts
 * the characters in each pair.
 *
 * Runs after the case modes, so it sees the keys that the case modes let pass, and
 * before the symbol holds, so a bracket hold makes a pair in place of a symbol.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event, with its tap count.
 * @return False for a push that sends other keys, and for the release of that key;
 *         true otherwise.
 */
bool process_bracket_pairs(uint16_t keycode, keyrecord_t *record);

/**
 * Counts a key that an earlier handler sent with tap_code16() in place of a push,
 * because this module does not see that push.
 *
 * @param keycode Sent keycode.
 */
void bracket_pairs_count_sent_key(uint16_t keycode);

/**
 * Takes back the count of a character push that a later handler took with no
 * output, because the host did not get the character.
 */
void bracket_pairs_take_back_count(void);

/** Removes all open pairs, for a mode that must not make pairs */
void bracket_pairs_clear(void);

// #endregion
