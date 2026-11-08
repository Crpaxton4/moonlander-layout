/** @file
 * Combos of the qgmlwy keymap, and the modes in which they operate.
 *
 * Each combo is a set of positions: LAYER_COMBO_POSITIONS holds a unique keycode
 * at each position, and `key_combos` in keymap.c lists those keycodes.
 */

#pragma once

// #region Enumerations

/** Combos, each the index of its entry in `key_combos` */
enum combo {
    LOGGING_COMBO,    /**< (1,6) and (1,7): logging on or off */
    PLAIN_MODE_COMBO, /**< (2,6) and (2,7): plain mode on or off */
};

// #endregion

// #region Functions

/**
 * Turns QMK's combos off while the nav mode is on, and on again after it. QMK finds
 * the combo keys on LAYER_COMBO_POSITIONS in each mode, but in the nav mode these
 * keys send nothing. With MOONLANDER_USER_LEDS the keyboard does not call
 * layer_state_set_user(), so the housekeeping task does this on each pass.
 */
void update_combos(void);

// #endregion
