/** @file
 * Double whitespace rule of the special mode, with no QMK code.
 *
 * special_mode.c gives each push in the mode as a key class, and ends the mode when
 * the rule tells it to.
 */

#pragma once

// #region Includes

#include <stdbool.h>

// #endregion

// #region Enumerations

/** Class of a pushed key, as the double whitespace rule sees it */
enum special_mode_key {
    SPECIAL_MODE_OTHER_KEY, /**< Each key that is not Space, Enter or Tab */
    SPECIAL_MODE_SPACE,     /**< Space */
    SPECIAL_MODE_ENTER,     /**< Enter */
    SPECIAL_MODE_TAB,       /**< Tab */
};

// #endregion

// #region Types

/** State of one special mode */
typedef struct {
    enum special_mode_key last_push; /**< Key class of the last push in the mode */
} special_mode_t;

// #endregion

// #region Functions

/**
 * Clears the last push, because a new mode starts.
 *
 * @param mode Mode state.
 */
void special_mode_start(special_mode_t *mode);

/**
 * Tells if a push ends the mode: the same whitespace key two times in a row.
 *
 * @param mode Mode state.
 * @param key  Key class of the push.
 * @return True if the push ends the mode and must send nothing.
 */
bool special_mode_push_ends_mode(special_mode_t *mode, enum special_mode_key key);

// #endregion
