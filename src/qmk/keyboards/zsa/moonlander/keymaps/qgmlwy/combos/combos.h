/** @file
 * Combos of the qgmlwy keymap.
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
