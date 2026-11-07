/** @file
 * One-shot and double whitespace rule of the special mode, with no QMK code.
 *
 * special_mode.c gives each push in the mode as a key class, and ends the mode when
 * the rules tell it to.
 */

#pragma once

// #region Enumerations

/** Class of a pushed key, as the double whitespace rule sees it */
enum special_mode_key {
    SPECIAL_MODE_OTHER_KEY, /**< Each key that is not Space, Enter or Tab */
    SPECIAL_MODE_SPACE,     /**< Space */
    SPECIAL_MODE_ENTER,     /**< Enter */
    SPECIAL_MODE_TAB,       /**< Tab */
};

/** Kind of a special mode, which sets when the mode ends */
enum special_mode_kind {
    SPECIAL_MODE_LOCKED,   /**< On until a mode key tap or a double whitespace key */
    SPECIAL_MODE_ONE_SHOT, /**< On for one push */
};

/** Result of one push in the mode */
enum special_mode_push_result {
    SPECIAL_MODE_STAYS_ON,         /**< Push sends its key, and the mode stays on */
    SPECIAL_MODE_ENDS_AFTER_KEY,   /**< Push sends its key, and the mode ends */
    SPECIAL_MODE_ENDS_WITH_NO_KEY, /**< Push sends nothing, and the mode ends */
};

// #endregion

// #region Types

/** State of one special mode */
typedef struct {
    enum special_mode_kind kind;      /**< Kind of the mode */
    enum special_mode_key  last_push; /**< Key class of the last push in the mode */
} special_mode_t;

// #endregion

// #region Functions

/**
 * Starts a mode of a kind, with no last push.
 *
 * @param mode Mode state.
 * @param kind Kind of the new mode.
 */
void special_mode_start(special_mode_t *mode, enum special_mode_kind kind);

/**
 * Tells what a push does to the mode. A one-shot ends after its push. A locked mode
 * ends on the same whitespace key two times in a row.
 *
 * @param mode Mode state.
 * @param key  Key class of the push.
 * @return Result of the push.
 */
enum special_mode_push_result special_mode_push(special_mode_t       *mode,
                                                enum special_mode_key key);

// #endregion
