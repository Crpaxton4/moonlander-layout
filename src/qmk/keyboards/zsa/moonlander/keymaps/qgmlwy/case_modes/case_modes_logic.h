/** @file
 * State machine of the case modes, with no QMK code.
 *
 * One machine serves all four modes, so all modes have the same end rules.
 * case_modes.c gives it the key class of each key push and does the action that it
 * returns.
 */

#pragma once

// #region Includes

#include <stdbool.h>

// #endregion

// #region Enumerations

/** Identifier style that the keyboard makes from words with Space between them */
enum case_mode {
    CASE_MODE_OFF = 0,  /**< No mode; 0, so a zero state has no mode at power-up */
    CASE_MODE_SNAKE,    /**< snake_case */
    CASE_MODE_CONSTANT, /**< CONSTANT_CASE */
    CASE_MODE_CAMEL,    /**< camelCase */
    CASE_MODE_PASCAL,   /**< PascalCase */
};

/** Kind of a pushed key, which selects what a mode does with it */
enum case_key_class {
    CASE_KEY_LETTER,    /**< Letter A to Z, or the tap of a tap-hold letter key */
    CASE_KEY_DIGIT,     /**< Digit with no Shift */
    CASE_KEY_SPACE,     /**< Space */
    CASE_KEY_BACKSPACE, /**< Backspace */
    CASE_KEY_MODIFIER,  /**< Modifier, or the hold of a mod-tap key */
    CASE_KEY_MODE,      /**< Key of a case mode */
    CASE_KEY_OTHER,     /**< Any other key, or a key with Ctrl, Alt or GUI held */
};

/** What the keyboard sends for one key push */
enum case_action {
    CASE_ACTION_SEND_AS_IS,                /**< Pushed key, with no change */
    CASE_ACTION_SEND_WITH_SHIFT,           /**< Pushed key with Shift, a capital */
    CASE_ACTION_SEND_UNDERSCORE,           /**< Underscore in place of the key */
    CASE_ACTION_SEND_NOTHING,              /**< No key */
    CASE_ACTION_SEND_BACKSPACE_THEN_SPACE, /**< Backspace, then the pushed Space */
};

/** Separator that a Space typed */
enum case_separator {
    CASE_SEPARATOR_NONE = 0,   /**< Last key was not the first Space after a word */
    CASE_SEPARATOR_UNDERSCORE, /**< Space typed an underscore */
    CASE_SEPARATOR_EMPTY,      /**< Space typed nothing */
};

// #endregion

// #region Types

/** One key push */
typedef struct {
    enum case_key_class key_class; /**< Kind of the key */
    enum case_mode      mode;      /**< Mode of a CASE_KEY_MODE key */
} case_key_t;

/** State of the case modes; all zero is the state at power-up, no mode */
typedef struct {
    enum case_mode      mode;               /**< Mode that is on */
    enum case_separator last_separator;     /**< Separator of the last key */
    bool                space_since_letter; /**< A Space came after the last letter */
    bool                letter_in_mode;     /**< A letter came after the mode started */
} case_modes_t;

// #endregion

// #region Functions

/**
 * Gives the action for one key push, and changes the mode as the push requires.
 *
 * @param case_modes Case mode state.
 * @param key        Pushed key.
 * @return Action that the keyboard must do for the push.
 */
enum case_action case_modes_push(case_modes_t *case_modes, case_key_t key);

/**
 * Ends the mode and clears the word state.
 *
 * @param case_modes Case mode state.
 */
void case_modes_end(case_modes_t *case_modes);

// #endregion
