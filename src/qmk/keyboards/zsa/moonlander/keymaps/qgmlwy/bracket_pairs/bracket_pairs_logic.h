/** @file
 * Pair stack of the bracket and quote pairs, with no QMK code.
 *
 * bracket_pairs.c gives it the key class of each key push and sends the outputs
 * that it returns. Each entry counts the characters in its pair, so Backspace and
 * the jump-out keep the stack the same as the host text.
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stdint.h>

// #endregion

// #region Macros

/** Most outputs of one push: opener, closer and Left */
#define PAIR_MOST_OUTPUTS 3

/** Most open pairs; a new pair removes the oldest entry of a full stack */
#define PAIR_STACK_CAPACITY 8

/** Time in ms with no key push after which the host cursor can have moved */
#define PAIR_IDLE_TIME 30000

/** Shortest hold time in ms of a key that the host can repeat */
#define PAIR_REPEAT_SAFE_TIME 400

// #endregion

// #region Enumerations

/** Kind of a pair; a quote is its own closer */
enum pair_kind {
    PAIR_PARENTHESES,     /**< ( and ) */
    PAIR_SQUARE_BRACKETS, /**< [ and ] */
    PAIR_CURLY_BRACES,    /**< { and } */
    PAIR_ANGLE_BRACKETS,  /**< < and > */
    PAIR_DOUBLE_QUOTES,   /**< Two double quotes */
    PAIR_BACKTICKS,       /**< Two backticks */
};

/** Kind of a pushed key, which selects what the pair stack does with it */
enum pair_key_class {
    PAIR_KEY_NO_CHARACTER, /**< Key that types nothing, for example a modifier */
    PAIR_KEY_CHARACTER,    /**< Key that types one character */
    PAIR_KEY_OPENER,       /**< Hold of an opening bracket */
    PAIR_KEY_LONE_OPENER,  /**< Hold of an opening bracket with Shift: no pair */
    PAIR_KEY_CLOSER,       /**< Hold of a closing bracket */
    PAIR_KEY_QUOTE,        /**< Double quote or backtick */
    PAIR_KEY_TAB,          /**< Tab */
    PAIR_KEY_BACKSPACE,    /**< Backspace */
    PAIR_KEY_LEFT,         /**< Left with no modifier */
    PAIR_KEY_RIGHT,        /**< Right with no modifier */
    PAIR_KEY_CLEARING,     /**< Key that can move the host cursor */
};

/** Key that the keyboard sends in place of the pushed key */
enum pair_output {
    PAIR_OUTPUT_OPENER,    /**< Opening character of the result kind */
    PAIR_OUTPUT_CLOSER,    /**< Closing character of the result kind */
    PAIR_OUTPUT_LEFT,      /**< Left arrow */
    PAIR_OUTPUT_RIGHT,     /**< Right arrow */
    PAIR_OUTPUT_BACKSPACE, /**< Backspace */
    PAIR_OUTPUT_DELETE,    /**< Delete */
};

// #endregion

// #region Types

/** One key push */
typedef struct {
    enum pair_key_class key_class; /**< Kind of the key */
    enum pair_kind      kind;      /**< Pair of an opener, closer or quote */
} pair_key_t;

/** What the keyboard does for one key push */
typedef struct {
    bool             passes;       /**< Pushed key goes on, with no outputs */
    enum pair_kind   kind;         /**< Pair of the opener and closer outputs */
    uint8_t          output_count; /**< Number of outputs */
    uint16_t         repeat_count; /**< Times to send all outputs again, in order */
    enum pair_output outputs[PAIR_MOST_OUTPUTS]; /**< Keys to send, in order */
} pair_result_t;

/** One open pair */
typedef struct {
    enum pair_kind kind;   /**< Kind of the pair */
    uint16_t       count;  /**< Characters that the host has in the pair */
    uint16_t       offset; /**< Characters between the cursor and the closer */
} pair_entry_t;

/** Open pairs, the innermost pair on top; all zero is an empty stack */
typedef struct {
    pair_entry_t entries[PAIR_STACK_CAPACITY]; /**< Open pairs, oldest first */
    uint8_t      depth;                        /**< Number of entries */
    uint32_t     last_push_time;               /**< Time of the last key push in ms */
} pair_stack_t;

// #endregion

// #region Functions

/**
 * Gives the result of one key push, and changes the stack as the push requires.
 *
 * @param stack Pair stack.
 * @param key   Pushed key.
 * @param now   Time of the push in ms.
 * @return Result of the push.
 */
pair_result_t pair_stack_push(pair_stack_t *stack, pair_key_t key, uint32_t now);

/**
 * Counts a key that an earlier handler sent in place of a push, so that each count
 * stays the same as the host text.
 *
 * @param stack Pair stack.
 * @param sent  Sent key; only PAIR_KEY_CHARACTER and PAIR_KEY_BACKSPACE change a
 *              count.
 * @param now   Time of the push in ms.
 */
void pair_stack_count_sent_key(pair_stack_t *stack, pair_key_t sent, uint32_t now);

/**
 * Removes all entries, for a host cursor that can have moved.
 *
 * @param stack Pair stack.
 */
void pair_stack_clear(pair_stack_t *stack);

/**
 * Clears the stack if the host can have repeated a key that passed, because each
 * repeat changes the host text.
 *
 * @param stack     Pair stack.
 * @param held_time Time in ms from the push to the release of the key.
 */
void pair_stack_release_passed_key(pair_stack_t *stack, uint32_t held_time);

/**
 * Tells if a pair is open, so that Tab and the closer jump out.
 *
 * @param stack Pair stack.
 * @param now   Time in ms.
 * @return False for an empty stack, or after the idle time with no key push.
 */
bool pair_stack_has_pairs(const pair_stack_t *stack, uint32_t now);

// #endregion
