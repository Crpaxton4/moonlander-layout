/** @file
 * State machine of the case modes.
 */

#include "case_modes/case_modes_logic.h"

/** Effect of one mode on letters and on Space */
typedef struct {
    bool all_capitals;             /**< Each letter is a capital */
    bool capital_after_space;      /**< First letter after a Space is a capital */
    bool capital_at_start;         /**< First letter of the mode is a capital */
    enum case_separator separator; /**< What the first Space after a word types */
} case_style_t;

/** Style of each mode, indexed by mode */
static const case_style_t case_styles[] = {
    [CASE_MODE_SNAKE] =
        {
            .separator = CASE_SEPARATOR_UNDERSCORE,
        },
    [CASE_MODE_CONSTANT] =
        {
            .all_capitals = true,
            .separator    = CASE_SEPARATOR_UNDERSCORE,
        },
    [CASE_MODE_CAMEL] =
        {
            .capital_after_space = true,
            .separator           = CASE_SEPARATOR_EMPTY,
        },
    [CASE_MODE_PASCAL] =
        {
            .capital_after_space = true,
            .capital_at_start    = true,
            .separator           = CASE_SEPARATOR_EMPTY,
        },
};

/**
 * Gives the action for a letter.
 *
 * @param case_modes Case mode state.
 * @return CASE_ACTION_SEND_WITH_SHIFT for a capital, CASE_ACTION_SEND_AS_IS else.
 */
static enum case_action push_letter(case_modes_t *case_modes) {
    const case_style_t *style  = &case_styles[case_modes->mode];
    enum case_action    action = CASE_ACTION_SEND_AS_IS;
    if (style->all_capitals ||
        (style->capital_after_space && case_modes->space_since_letter) ||
        (style->capital_at_start && !case_modes->letter_in_mode)) {
        action = CASE_ACTION_SEND_WITH_SHIFT;
    }
    case_modes->space_since_letter = false;
    case_modes->letter_in_mode     = true;
    case_modes->last_separator     = CASE_SEPARATOR_NONE;
    return action;
}

/**
 * Gives the action for a digit or Backspace, which pass. Either key ends a Space
 * pair, so the next Space types a separator again.
 *
 * @param case_modes Case mode state.
 * @return CASE_ACTION_SEND_AS_IS.
 */
static enum case_action push_digit_or_backspace(case_modes_t *case_modes) {
    case_modes->last_separator = CASE_SEPARATOR_NONE;
    return CASE_ACTION_SEND_AS_IS;
}

void case_modes_end(case_modes_t *case_modes) {
    *case_modes = (case_modes_t){.mode = CASE_MODE_OFF};
}

/**
 * Gives the action for a Space. The first Space after a word types the separator of
 * the mode. A second Space ends the mode and leaves one Space with no separator.
 *
 * @param case_modes Case mode state.
 * @return Action for the Space.
 */
static enum case_action push_space(case_modes_t *case_modes) {
    enum case_separator previous = case_modes->last_separator;
    if (previous == CASE_SEPARATOR_UNDERSCORE) {
        case_modes_end(case_modes);
        return CASE_ACTION_SEND_BACKSPACE_THEN_SPACE;
    } else if (previous == CASE_SEPARATOR_EMPTY) {
        case_modes_end(case_modes);
        return CASE_ACTION_SEND_AS_IS;
    }
    enum case_separator separator  = case_styles[case_modes->mode].separator;
    case_modes->last_separator     = separator;
    case_modes->space_since_letter = true;
    if (separator == CASE_SEPARATOR_UNDERSCORE) {
        return CASE_ACTION_SEND_UNDERSCORE;
    }
    return CASE_ACTION_SEND_NOTHING;
}

/**
 * Starts or changes to the mode of a mode key, or ends it if it is already on.
 *
 * A change keeps the word state, so the next Space or letter acts on the text that
 * the last mode typed. With no mode on, the word state is all zero.
 *
 * @param case_modes Case mode state.
 * @param mode       Mode of the key.
 * @return CASE_ACTION_SEND_NOTHING.
 */
static enum case_action push_mode_key(case_modes_t *case_modes, enum case_mode mode) {
    if (mode == case_modes->mode) {
        case_modes_end(case_modes);
    } else {
        case_modes->mode           = mode;
        case_modes->letter_in_mode = false;
    }
    return CASE_ACTION_SEND_NOTHING;
}

/**
 * Gives the action for a modifier, which passes and keeps the word state, because
 * a modifier alone types nothing.
 *
 * @param case_modes Case mode state.
 * @return CASE_ACTION_SEND_AS_IS.
 */
static enum case_action push_modifier(case_modes_t *case_modes) {
    (void)case_modes;
    return CASE_ACTION_SEND_AS_IS;
}

/**
 * Ends the mode for any other key, which passes.
 *
 * @param case_modes Case mode state.
 * @return CASE_ACTION_SEND_AS_IS.
 */
static enum case_action push_other_key(case_modes_t *case_modes) {
    case_modes_end(case_modes);
    return CASE_ACTION_SEND_AS_IS;
}

/** Gives the action for one key push while a mode is on, and changes the state */
typedef enum case_action (*key_handler_t)(case_modes_t *case_modes);

/**
 * Handler of each key class while a mode is on. A mode key needs its mode, so
 * case_modes_push() handles it.
 */
static const key_handler_t key_handlers[] = {
    [CASE_KEY_LETTER] = push_letter,     [CASE_KEY_DIGIT] = push_digit_or_backspace,
    [CASE_KEY_SPACE] = push_space,       [CASE_KEY_BACKSPACE] = push_digit_or_backspace,
    [CASE_KEY_MODIFIER] = push_modifier, [CASE_KEY_OTHER] = push_other_key,
};

enum case_action case_modes_push(case_modes_t *case_modes, case_key_t key) {
    if (key.key_class == CASE_KEY_MODE) {
        return push_mode_key(case_modes, key.mode);
    } else if (case_modes->mode == CASE_MODE_OFF) {
        return CASE_ACTION_SEND_AS_IS;
    }
    return key_handlers[key.key_class](case_modes);
}
