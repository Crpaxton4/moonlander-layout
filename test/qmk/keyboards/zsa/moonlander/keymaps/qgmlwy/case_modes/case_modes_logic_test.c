/** @file
 * Tests of the case mode state machine, with no QMK code.
 *
 * A small editor model applies each action to a text, so each case compares the
 * text that the host editor shows with the text of the acceptance check.
 */

// #region Includes

#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "case_modes/case_modes_logic.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Characters that the editor model can keep, with the terminating zero */
#define EDITOR_CAPACITY 64

/** Number of modes in all_modes */
#define MODE_COUNT (sizeof(all_modes) / sizeof(all_modes[0]))

// #endregion

// #region Types

/** Text in the editor model of the host */
typedef struct {
    char   text[EDITOR_CAPACITY]; /**< Text, with a terminating zero */
    size_t length;                /**< Characters in text */
} editor_t;

/** Keys that a case types, and the text that the editor must then show */
typedef struct {
    const char *typed; /**< Characters of the pushed keys, in push order */
    const char *shown; /**< Text that the editor must show */
} typing_t;

/** Typing in one mode, a change to a second mode, and more typing */
typedef struct {
    enum case_mode first_mode;   /**< Mode to start */
    const char    *first_typed;  /**< Characters of the keys in the first mode */
    enum case_mode second_mode;  /**< Mode to change to */
    const char    *second_typed; /**< Characters of the keys after the change */
    const char    *shown;        /**< Text that the editor must show */
} mode_change_t;

// #endregion

// #region Data

/** Each case mode, for cases that cover all four */
static const enum case_mode all_modes[] = {
    CASE_MODE_SNAKE,
    CASE_MODE_CONSTANT,
    CASE_MODE_CAMEL,
    CASE_MODE_PASCAL,
};

// #endregion

// #region Static functions

/**
 * Types one character in the editor model, as the host does for a key.
 *
 * @param editor    Editor model.
 * @param character Character; '\b' deletes the last character.
 */
static void type_character(editor_t *editor, char character) {
    if (character == '\b' && editor->length > 0) {
        editor->length--;
    } else if (character != '\b') {
        assert_true(editor->length < EDITOR_CAPACITY - 1);
        editor->text[editor->length] = character;
        editor->length++;
    }
    editor->text[editor->length] = '\0';
}

/**
 * Gives the key class of a character of a test text.
 *
 * @param character Lower-case letter, digit, ' ', '\b' or any other character.
 * @return Key class of the key that types the character.
 */
static case_key_t key_of_character(char character) {
    if (islower((unsigned char)character)) {
        return (case_key_t){.key_class = CASE_KEY_LETTER};
    } else if (isdigit((unsigned char)character)) {
        return (case_key_t){.key_class = CASE_KEY_DIGIT};
    } else if (character == ' ') {
        return (case_key_t){.key_class = CASE_KEY_SPACE};
    } else if (character == '\b') {
        return (case_key_t){.key_class = CASE_KEY_BACKSPACE};
    }
    return (case_key_t){.key_class = CASE_KEY_OTHER};
}

/**
 * Applies the action of one key push to the editor model.
 *
 * @param action    Action that the case modes selected.
 * @param editor    Editor model.
 * @param character Character of the pushed key.
 */
static void apply_action(enum case_action action, editor_t *editor, char character) {
    switch (action) {
        case CASE_ACTION_SEND_AS_IS:
            type_character(editor, character);
            break;
        case CASE_ACTION_SEND_WITH_SHIFT:
            type_character(editor, (char)toupper((unsigned char)character));
            break;
        case CASE_ACTION_SEND_UNDERSCORE:
            type_character(editor, '_');
            break;
        case CASE_ACTION_SEND_NOTHING:
            break;
        case CASE_ACTION_SEND_BACKSPACE_THEN_SPACE:
            type_character(editor, '\b');
            type_character(editor, ' ');
            break;
    }
}

/**
 * Pushes the key of each character of a text, and applies each action to the
 * editor model.
 *
 * @param case_modes Case mode state.
 * @param editor     Editor model.
 * @param text       Characters of the keys, in push order.
 */
static void type_text(case_modes_t *case_modes, editor_t *editor, const char *text) {
    for (size_t index = 0; text[index] != '\0'; index++) {
        enum case_action action =
            case_modes_push(case_modes, key_of_character(text[index]));
        apply_action(action, editor, text[index]);
    }
}

/**
 * Pushes the key of a case mode.
 *
 * @param case_modes Case mode state.
 * @param mode       Mode of the key.
 * @return Action for the push.
 */
static enum case_action push_mode_key(case_modes_t *case_modes, enum case_mode mode) {
    case_key_t mode_key = {.key_class = CASE_KEY_MODE, .mode = mode};
    return case_modes_push(case_modes, mode_key);
}

/**
 * Starts a mode, types a text, and fails the test if the editor model does not
 * show the expected text or the mode is not on.
 *
 * @param mode   Mode to start.
 * @param typing Typed keys and the text that the editor must show.
 */
static void assert_mode_types(enum case_mode mode, const typing_t *typing) {
    case_modes_t case_modes = {0};
    editor_t     editor     = {0};
    push_mode_key(&case_modes, mode);
    type_text(&case_modes, &editor, typing->typed);
    assert_string_equal(editor.text, typing->shown);
    assert_int_equal(case_modes.mode, mode);
}

/**
 * Starts a mode, types a text, and fails the test if the editor model does not
 * show the expected text or a mode is still on.
 *
 * @param mode   Mode to start.
 * @param typing Typed keys and the text that the editor must show.
 */
static void assert_mode_ends(enum case_mode mode, const typing_t *typing) {
    case_modes_t case_modes = {0};
    editor_t     editor     = {0};
    push_mode_key(&case_modes, mode);
    type_text(&case_modes, &editor, typing->typed);
    assert_string_equal(editor.text, typing->shown);
    assert_int_equal(case_modes.mode, CASE_MODE_OFF);
}

/**
 * Types in one mode, changes to a second mode and types more. Fails the test if
 * the editor model does not show the expected text or the second mode is not on.
 *
 * @param change Modes, typed keys and the text that the editor must show.
 */
static void assert_mode_change(const mode_change_t *change) {
    case_modes_t case_modes = {0};
    editor_t     editor     = {0};
    push_mode_key(&case_modes, change->first_mode);
    type_text(&case_modes, &editor, change->first_typed);
    enum case_action action = push_mode_key(&case_modes, change->second_mode);
    assert_int_equal(action, CASE_ACTION_SEND_NOTHING);
    type_text(&case_modes, &editor, change->second_typed);
    assert_string_equal(editor.text, change->shown);
    assert_int_equal(case_modes.mode, change->second_mode);
}

static void case_modes_logic_r10_zero_state_has_no_mode(void **state) {
    (void)state;
    case_modes_t case_modes = {0};
    assert_int_equal(case_modes.mode, CASE_MODE_OFF);
    editor_t editor = {0};
    type_text(&case_modes, &editor, "a b");
    assert_string_equal(editor.text, "a b");
    assert_int_equal(case_modes.mode, CASE_MODE_OFF);
}

static void case_modes_logic_r1_mode_key_starts_its_mode(void **state) {
    (void)state;
    for (size_t index = 0; index < MODE_COUNT; index++) {
        case_modes_t     case_modes = {0};
        enum case_action action     = push_mode_key(&case_modes, all_modes[index]);
        assert_int_equal(action, CASE_ACTION_SEND_NOTHING);
        assert_int_equal(case_modes.mode, all_modes[index]);
    }
}

static void case_modes_logic_r2_ac1_snake_case(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "max retry count",
        .shown = "max_retry_count",
    };
    assert_mode_types(CASE_MODE_SNAKE, &typing);
}

static void case_modes_logic_r2_ac2_constant_case(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "max retry",
        .shown = "MAX_RETRY",
    };
    assert_mode_types(CASE_MODE_CONSTANT, &typing);
}

static void case_modes_logic_r2_ac3_camel_case(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "max retry count",
        .shown = "maxRetryCount",
    };
    assert_mode_types(CASE_MODE_CAMEL, &typing);
}

static void case_modes_logic_r2_ac4_pascal_case(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "max retry count",
        .shown = "MaxRetryCount",
    };
    assert_mode_types(CASE_MODE_PASCAL, &typing);
}

static void case_modes_logic_r3_ac7_digit_passes(void **state) {
    (void)state;
    static const typing_t ac7_typing = {
        .typed = "item 2",
        .shown = "item_2",
    };
    assert_mode_types(CASE_MODE_SNAKE, &ac7_typing);
    // Digit is a word character, so the next Space is a first Space again
    static const typing_t next_word_typing = {
        .typed = "item 2 x",
        .shown = "item_2_x",
    };
    assert_mode_types(CASE_MODE_SNAKE, &next_word_typing);
}

static void case_modes_logic_r4_ac8_backspace_keeps_mode(void **state) {
    (void)state;
    static const typing_t ac8_typing = {
        .typed = "max ret\bx",
        .shown = "max_rex",
    };
    assert_mode_types(CASE_MODE_SNAKE, &ac8_typing);
    // Backspace deletes the underscore, so the next Space types it again
    static const typing_t underscore_typing = {
        .typed = "max \b count",
        .shown = "max_count",
    };
    assert_mode_types(CASE_MODE_SNAKE, &underscore_typing);
}

static void case_modes_logic_r5_ac5_double_space_after_underscore(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "max retry  x",
        .shown = "max_retry x",
    };
    assert_mode_ends(CASE_MODE_SNAKE, &typing);
}

static void case_modes_logic_r5_double_space_after_empty_separator(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "get value  x",
        .shown = "getValue x",
    };
    assert_mode_ends(CASE_MODE_CAMEL, &typing);
}

static void case_modes_logic_r6_ac6_other_key_ends_mode(void **state) {
    (void)state;
    static const typing_t typing = {
        .typed = "get value( x",
        .shown = "getValue( x",
    };
    assert_mode_ends(CASE_MODE_CAMEL, &typing);
}

static void case_modes_logic_r7_ac9_same_mode_key_ends_mode(void **state) {
    (void)state;
    case_modes_t case_modes = {0};
    editor_t     editor     = {0};
    push_mode_key(&case_modes, CASE_MODE_PASCAL);
    enum case_action action = push_mode_key(&case_modes, CASE_MODE_PASCAL);
    assert_int_equal(action, CASE_ACTION_SEND_NOTHING);
    type_text(&case_modes, &editor, "a b");
    assert_string_equal(editor.text, "a b");
    assert_int_equal(case_modes.mode, CASE_MODE_OFF);
}

static void case_modes_logic_r8_ac10_other_mode_key_changes_mode(void **state) {
    (void)state;
    static const mode_change_t ac10_change = {
        .first_mode   = CASE_MODE_SNAKE,
        .first_typed  = "max",
        .second_mode  = CASE_MODE_CONSTANT,
        .second_typed = " count",
        .shown        = "max_COUNT",
    };
    assert_mode_change(&ac10_change);
    // Change keeps the word state: a Space before it still counts
    static const mode_change_t after_space_change = {
        .first_mode   = CASE_MODE_SNAKE,
        .first_typed  = "max ",
        .second_mode  = CASE_MODE_CAMEL,
        .second_typed = "count",
        .shown        = "max_Count",
    };
    assert_mode_change(&after_space_change);
}

static void case_modes_logic_r9_modifier_keeps_mode(void **state) {
    (void)state;
    case_modes_t case_modes = {0};
    editor_t     editor     = {0};
    push_mode_key(&case_modes, CASE_MODE_SNAKE);
    type_text(&case_modes, &editor, "max ");
    // Modifier between the two Spaces does not break the Space pair
    case_key_t       modifier = {.key_class = CASE_KEY_MODIFIER};
    enum case_action action   = case_modes_push(&case_modes, modifier);
    assert_int_equal(action, CASE_ACTION_SEND_AS_IS);
    assert_int_equal(case_modes.mode, CASE_MODE_SNAKE);
    type_text(&case_modes, &editor, " x");
    assert_string_equal(editor.text, "max x");
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest case_modes_logic_cases[] = {
    cmocka_unit_test(case_modes_logic_r10_zero_state_has_no_mode),
    cmocka_unit_test(case_modes_logic_r1_mode_key_starts_its_mode),
    cmocka_unit_test(case_modes_logic_r2_ac1_snake_case),
    cmocka_unit_test(case_modes_logic_r2_ac2_constant_case),
    cmocka_unit_test(case_modes_logic_r2_ac3_camel_case),
    cmocka_unit_test(case_modes_logic_r2_ac4_pascal_case),
    cmocka_unit_test(case_modes_logic_r3_ac7_digit_passes),
    cmocka_unit_test(case_modes_logic_r4_ac8_backspace_keeps_mode),
    cmocka_unit_test(case_modes_logic_r5_ac5_double_space_after_underscore),
    cmocka_unit_test(case_modes_logic_r5_double_space_after_empty_separator),
    cmocka_unit_test(case_modes_logic_r6_ac6_other_key_ends_mode),
    cmocka_unit_test(case_modes_logic_r7_ac9_same_mode_key_ends_mode),
    cmocka_unit_test(case_modes_logic_r8_ac10_other_mode_key_changes_mode),
    cmocka_unit_test(case_modes_logic_r9_modifier_keeps_mode),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(case_modes_logic_cases, NULL, NULL);
}

// #endregion
