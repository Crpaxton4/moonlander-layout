/** @file
 * Pair stack of the bracket and quote pairs.
 */

// #region Includes

#include "bracket_pairs/bracket_pairs_logic.h"

// #endregion

// #region Macros

/** Characters of one pair: the opener and the closer */
#define PAIR_CHARACTERS 2

/** Index step from one entry to the entry above it */
#define ONE_ENTRY 1

// #endregion

// #region Types

/**
 * Gives the result of one key push of a key class, and changes the stack.
 *
 * @param stack Pair stack.
 * @param kind  Pair of an opener, closer or quote.
 * @return Result of the push.
 */
typedef pair_result_t (*key_handler_t)(pair_stack_t *stack, enum pair_kind kind);

// #endregion

// #region Function declarations

static pair_result_t push_no_character(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_character(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_lone_opener(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t open_pair(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_tab(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_closer(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_quote(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_backspace(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_left(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_right(pair_stack_t *stack, enum pair_kind kind);
static pair_result_t push_clearing_key(pair_stack_t *stack, enum pair_kind kind);

// #endregion

// #region Data

/** Result of a key that goes on with no change */
static const pair_result_t pass_result = {.passes = true};

/** Result of a new pair, with no kind yet: the pair, then Left into it */
static const pair_result_t open_pair_result = {
    .output_count = 3,
    .outputs      = {PAIR_OUTPUT_OPENER, PAIR_OUTPUT_CLOSER, PAIR_OUTPUT_LEFT},
};

/** Result of a lone opener, with no kind yet: the opener only */
static const pair_result_t lone_opener_result = {
    .output_count = 1,
    .outputs      = {PAIR_OUTPUT_OPENER},
};

/** Result of a jump-out with the cursor at the closer: Right, past the closer */
static const pair_result_t jump_out_result = {
    .output_count = 1,
    .outputs      = {PAIR_OUTPUT_RIGHT},
};

/** Result of a delete of an empty pair: Backspace, then Delete */
static const pair_result_t delete_pair_result = {
    .output_count = 2,
    .outputs      = {PAIR_OUTPUT_BACKSPACE, PAIR_OUTPUT_DELETE},
};

/** Handler of each key class, indexed by key class */
static const key_handler_t key_handlers[] = {
    [PAIR_KEY_NO_CHARACTER] = push_no_character,
    [PAIR_KEY_CHARACTER]    = push_character,
    [PAIR_KEY_OPENER]       = open_pair,
    [PAIR_KEY_LONE_OPENER]  = push_lone_opener,
    [PAIR_KEY_CLOSER]       = push_closer,
    [PAIR_KEY_QUOTE]        = push_quote,
    [PAIR_KEY_TAB]          = push_tab,
    [PAIR_KEY_BACKSPACE]    = push_backspace,
    [PAIR_KEY_LEFT]         = push_left,
    [PAIR_KEY_RIGHT]        = push_right,
    [PAIR_KEY_CLEARING]     = push_clearing_key,
};

// #endregion

// #region Static functions

/**
 * Gives the top entry of a stack that is not empty.
 *
 * @param stack Pair stack with one entry or more.
 * @return Top entry.
 */
static pair_entry_t *top_entry(pair_stack_t *stack) {
    return &stack->entries[stack->depth - ONE_ENTRY];
}

/**
 * Gives the characters between the opener of a pair and the cursor.
 *
 * @param entry Entry of the pair.
 * @return Count minus offset.
 */
static uint16_t characters_before_cursor(const pair_entry_t *entry) {
    return (uint16_t)(entry->count - entry->offset);
}

/**
 * Lets a key that types nothing pass.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result that lets the key pass.
 */
static pair_result_t push_no_character(pair_stack_t *stack, enum pair_kind kind) {
    (void)stack;
    (void)kind;
    return pass_result;
}

/**
 * Counts a character in the innermost pair, and lets the key pass.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result that lets the key pass.
 */
static pair_result_t push_character(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    if (stack->depth > 0) {
        top_entry(stack)->count++;
    }
    return pass_result;
}

/**
 * Counts a lone opener in the innermost pair. The module sends the opener itself,
 * because a held Shift would change "[" into "{" on the host.
 *
 * @param stack Pair stack.
 * @param kind  Pair kind of the opener.
 * @return Result that sends the opener only.
 */
static pair_result_t push_lone_opener(pair_stack_t *stack, enum pair_kind kind) {
    (void)push_character(stack, kind);
    pair_result_t result = lone_opener_result;
    result.kind          = kind;
    return result;
}

/**
 * Removes the bottom entry and moves each other entry down by one.
 *
 * @param stack Pair stack with one entry or more.
 */
static void remove_oldest_entry(pair_stack_t *stack) {
    stack->depth--;
    for (uint8_t index = 0; index < stack->depth; index++) {
        stack->entries[index] = stack->entries[index + ONE_ENTRY];
    }
}

/**
 * Puts a new pair with no characters on the stack.
 *
 * @param stack Pair stack.
 * @param kind  Kind of the pair.
 * @return Result that sends the pair and puts the cursor between its characters.
 */
static pair_result_t open_pair(pair_stack_t *stack, enum pair_kind kind) {
    if (stack->depth == PAIR_STACK_CAPACITY) {
        remove_oldest_entry(stack);
    }
    stack->entries[stack->depth] = (pair_entry_t){.kind = kind};
    stack->depth++;
    pair_result_t result = open_pair_result;
    result.kind          = kind;
    return result;
}

/**
 * Removes the top entry with the cursor after its closer. Adds its characters and
 * its two pair characters to the new top entry. The offset of the new top entry
 * stays, because the characters after the cursor did not change.
 *
 * @param stack Pair stack with one entry or more.
 */
static void leave_top_pair(pair_stack_t *stack) {
    uint16_t removed_count = top_entry(stack)->count;
    stack->depth--;
    if (stack->depth > 0) {
        pair_entry_t *new_top = top_entry(stack);
        new_top->count = (uint16_t)(new_top->count + removed_count + PAIR_CHARACTERS);
    }
}

/**
 * Moves the cursor past the closer of the innermost pair, and removes its entry.
 *
 * @param stack Pair stack with one entry or more.
 * @return Result that sends Right one time more than the offset, in place of the
 *         key.
 */
static pair_result_t jump_out(pair_stack_t *stack) {
    pair_result_t result = jump_out_result;
    result.repeat_count  = top_entry(stack)->offset;
    leave_top_pair(stack);
    return result;
}

/**
 * Jumps out of the innermost pair, or lets Tab pass with no pair.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result of the push.
 */
static pair_result_t push_tab(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    if (stack->depth == 0) {
        return pass_result;
    }
    return jump_out(stack);
}

/**
 * Jumps out of the innermost pair for its own closer. Any other closer is one
 * character in the pair, for example the ">" of "->". With no pair, the closer
 * passes.
 *
 * @param stack Pair stack.
 * @param kind  Pair of the closer.
 * @return Result of the push.
 */
static pair_result_t push_closer(pair_stack_t *stack, enum pair_kind kind) {
    if (stack->depth == 0) {
        return pass_result;
    } else if (top_entry(stack)->kind != kind) {
        return push_character(stack, kind);
    }
    return jump_out(stack);
}

/**
 * Jumps out of the innermost pair if it is a pair of the same quote, because a
 * quote is its own closer. Otherwise the quote opens a new pair.
 *
 * @param stack Pair stack.
 * @param kind  Pair of the quote.
 * @return Result of the push.
 */
static pair_result_t push_quote(pair_stack_t *stack, enum pair_kind kind) {
    if (stack->depth > 0 && top_entry(stack)->kind == kind) {
        return jump_out(stack);
    }
    return open_pair(stack, kind);
}

/**
 * Changes the stack for a Backspace that the host gets.
 *
 * @param stack Pair stack with one entry or more.
 */
static void count_backspace(pair_stack_t *stack) {
    pair_entry_t *top = top_entry(stack);
    if (characters_before_cursor(top) > 0) {
        top->count--;
    } else if (top->count > 0) {
        // Backspace deleted the opener, and the closer is not next to the cursor
        pair_stack_clear(stack);
    } else {
        // Backspace deleted the opener of an empty pair
        stack->depth--;
    }
}

/**
 * Lets Backspace delete one character in the innermost pair. In an empty pair,
 * Backspace deletes the opener and Delete deletes the closer, so no stray closer
 * stays.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result of the push.
 */
static pair_result_t push_backspace(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    if (stack->depth == 0) {
        return pass_result;
    } else if (top_entry(stack)->count == 0) {
        stack->depth--;
        return delete_pair_result;
    }
    count_backspace(stack);
    return pass_result;
}

/**
 * Counts a move of the cursor to the left in the innermost pair. A move past the
 * opener clears the stack, because the cursor is then outside each known place.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result that lets the key pass.
 */
static pair_result_t push_left(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    if (stack->depth == 0) {
        return pass_result;
    } else if (characters_before_cursor(top_entry(stack)) > 0) {
        top_entry(stack)->offset++;
        return pass_result;
    }
    pair_stack_clear(stack);
    return pass_result;
}

/**
 * Counts a move of the cursor to the right in the innermost pair. A move past the
 * closer leaves the pair, as a jump-out does.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result that lets the key pass.
 */
static pair_result_t push_right(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    if (stack->depth == 0) {
        return pass_result;
    } else if (top_entry(stack)->offset > 0) {
        top_entry(stack)->offset--;
        return pass_result;
    }
    leave_top_pair(stack);
    return pass_result;
}

/**
 * Clears the stack for a key that can move the host cursor, and lets the key pass.
 *
 * @param stack Pair stack.
 * @param kind  Not used.
 * @return Result that lets the key pass.
 */
static pair_result_t push_clearing_key(pair_stack_t *stack, enum pair_kind kind) {
    (void)kind;
    pair_stack_clear(stack);
    return pass_result;
}

/**
 * Clears the stack if no key push occurred for the idle time, and keeps the time of
 * this push.
 *
 * @param stack Pair stack.
 * @param now   Time of the push in ms.
 */
static void note_push_time(pair_stack_t *stack, uint32_t now) {
    if (now - stack->last_push_time >= PAIR_IDLE_TIME) {
        pair_stack_clear(stack);
    }
    stack->last_push_time = now;
}

// #endregion

// #region Public functions

void pair_stack_clear(pair_stack_t *stack) {
    stack->depth = 0;
}

pair_result_t pair_stack_push(pair_stack_t *stack, pair_key_t key, uint32_t now) {
    note_push_time(stack, now);
    return key_handlers[key.key_class](stack, key.kind);
}

void pair_stack_count_sent_key(pair_stack_t *stack, pair_key_t sent, uint32_t now) {
    note_push_time(stack, now);
    if (stack->depth == 0) {
        return;
    } else if (sent.key_class == PAIR_KEY_CHARACTER) {
        top_entry(stack)->count++;
    } else if (sent.key_class == PAIR_KEY_BACKSPACE) {
        count_backspace(stack);
    }
}

void pair_stack_release_passed_key(pair_stack_t *stack, uint32_t held_time) {
    if (held_time >= PAIR_REPEAT_SAFE_TIME) {
        pair_stack_clear(stack);
    }
}

bool pair_stack_has_pairs(const pair_stack_t *stack, uint32_t now) {
    if (stack->depth == 0) {
        return false;
    }
    // Next push clears the stack after the idle time, so the pairs are gone now
    return now - stack->last_push_time < PAIR_IDLE_TIME;
}

// #endregion
