/** @file
 * Tests of the pair stack, with no QMK code.
 *
 * An editor model with a text cursor applies each key that passes and each output,
 * so each case compares the text that the host editor shows with the text of the
 * acceptance check. A '|' in an expected text is the cursor.
 */

// #region Includes

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "bracket_pairs/bracket_pairs_logic.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Characters that the editor model can keep, with the terminating zero */
#define EDITOR_CAPACITY 64

/** Number of pair kinds */
#define PAIR_KIND_COUNT (sizeof(pair_characters) / sizeof(pair_characters[0]))

/** First pair kind that is a quote; the kinds from it up are quotes */
#define FIRST_QUOTE_KIND PAIR_DOUBLE_QUOTES

/** Character of a test text that pushes Enter, a key that clears the stack */
#define ENTER_CHARACTER '\n'

/** Character of a test text that pushes a modifier, which types nothing */
#define MODIFIER_CHARACTER '^'

/** Length of one character, for the moves of the text after the cursor */
#define ONE_CHARACTER 1

/** Index step from the stack depth to the top entry */
#define ONE_ENTRY 1

/** Time in ms between two pushes that does not clear the stack */
#define JUST_BEFORE_IDLE_TIME (PAIR_IDLE_TIME - 1)

/** Character of a test text that pushes a plain Left */
#define LEFT_CHARACTER '\002'

/** Character of a test text that pushes a plain Right */
#define RIGHT_CHARACTER '\006'

/** Number of test text characters of keys that type no character of the text */
#define KEY_CHARACTER_COUNT (sizeof(key_characters) / sizeof(key_characters[0]))

/** Hold time in ms of a released key that the host does not repeat */
#define JUST_BEFORE_REPEAT_SAFE_TIME (PAIR_REPEAT_SAFE_TIME - 1)

// #endregion

// #region Types

/** Text in the editor model of the host, with a text cursor */
typedef struct {
    char   text[EDITOR_CAPACITY]; /**< Text, with a terminating zero */
    size_t length;                /**< Characters in text */
    size_t cursor;                /**< Characters before the cursor */
} editor_t;

/** Pair stack, editor model and clock of one case */
typedef struct {
    pair_stack_t stack;  /**< Pair stack under test */
    editor_t     editor; /**< Text that the host shows */
    uint32_t     now;    /**< Time of the next push in ms */
} typing_t;

/** Opening and closing character of each pair kind */
typedef struct {
    char opener; /**< Character that opens the pair */
    char closer; /**< Character that closes the pair */
} pair_characters_t;

/** Character of a test text for a key that types no character of the text */
typedef struct {
    char                character; /**< Character in the test text */
    enum pair_key_class key_class; /**< Key class of the pushed key */
} key_character_t;

// #endregion

// #region Data

/** Characters of each pair kind, indexed by kind */
static const pair_characters_t pair_characters[] = {
    [PAIR_PARENTHESES]     = {.opener = '(', .closer = ')'},
    [PAIR_SQUARE_BRACKETS] = {.opener = '[', .closer = ']'},
    [PAIR_CURLY_BRACES]    = {.opener = '{', .closer = '}'},
    [PAIR_ANGLE_BRACKETS]  = {.opener = '<', .closer = '>'},
    [PAIR_DOUBLE_QUOTES]   = {.opener = '"', .closer = '"'},
    [PAIR_BACKTICKS]       = {.opener = '`', .closer = '`'},
};

/** Characters of the keys that type no character of the test text */
static const key_character_t key_characters[] = {
    {.character = '\t', .key_class = PAIR_KEY_TAB},
    {.character = '\b', .key_class = PAIR_KEY_BACKSPACE},
    {.character = ENTER_CHARACTER, .key_class = PAIR_KEY_CLEARING},
    {.character = MODIFIER_CHARACTER, .key_class = PAIR_KEY_NO_CHARACTER},
    {.character = LEFT_CHARACTER, .key_class = PAIR_KEY_LEFT},
    {.character = RIGHT_CHARACTER, .key_class = PAIR_KEY_RIGHT},
};

// #endregion

// #region Static functions

/**
 * Types one character at the cursor of the editor model.
 *
 * @param editor    Editor model.
 * @param character Character to insert.
 */
static void insert_character(editor_t *editor, char character) {
    assert_true(editor->length + ONE_CHARACTER < EDITOR_CAPACITY);
    // Text after the cursor and its terminating zero move one place right
    char *cursor_text = &editor->text[editor->cursor];
    memmove(cursor_text + ONE_CHARACTER, cursor_text,
            editor->length - editor->cursor + ONE_CHARACTER);
    editor->text[editor->cursor] = character;
    editor->length++;
    editor->cursor++;
}

/**
 * Deletes the character before the cursor, as Backspace does.
 *
 * @param editor Editor model.
 */
static void delete_before_cursor(editor_t *editor) {
    assert_true(editor->cursor > 0);
    editor->cursor--;
    char *cursor_text = &editor->text[editor->cursor];
    memmove(cursor_text, cursor_text + ONE_CHARACTER, editor->length - editor->cursor);
    editor->length--;
}

/**
 * Deletes the character after the cursor, as Delete does.
 *
 * @param editor Editor model.
 */
static void delete_after_cursor(editor_t *editor) {
    assert_true(editor->cursor < editor->length);
    char *cursor_text = &editor->text[editor->cursor];
    memmove(cursor_text, cursor_text + ONE_CHARACTER, editor->length - editor->cursor);
    editor->length--;
}

/**
 * Applies one output of the pair stack to the editor model.
 *
 * @param editor Editor model.
 * @param result Result with the output and the pair kind.
 * @param index  Index of the output in the result.
 */
static void apply_output(editor_t *editor, const pair_result_t *result, size_t index) {
    enum pair_kind kind = result->kind;
    switch (result->outputs[index]) {
        case PAIR_OUTPUT_OPENER:
            insert_character(editor, pair_characters[kind].opener);
            break;
        case PAIR_OUTPUT_CLOSER:
            insert_character(editor, pair_characters[kind].closer);
            break;
        case PAIR_OUTPUT_LEFT:
            assert_true(editor->cursor > 0);
            editor->cursor--;
            break;
        case PAIR_OUTPUT_RIGHT:
            assert_true(editor->cursor < editor->length);
            editor->cursor++;
            break;
        case PAIR_OUTPUT_BACKSPACE:
            delete_before_cursor(editor);
            break;
        case PAIR_OUTPUT_DELETE:
            delete_after_cursor(editor);
            break;
    }
}

/**
 * Applies a key that passes to the editor model, as the host does for its key.
 *
 * @param editor    Editor model.
 * @param character Character of the key in the test text.
 */
static void apply_passed_key(editor_t *editor, char character) {
    if (character == '\b') {
        delete_before_cursor(editor);
    } else if (character == LEFT_CHARACTER) {
        assert_true(editor->cursor > 0);
        editor->cursor--;
    } else if (character == RIGHT_CHARACTER) {
        assert_true(editor->cursor < editor->length);
        editor->cursor++;
    } else if (character != MODIFIER_CHARACTER) {
        insert_character(editor, character);
    }
}

/**
 * Gives the pair key of an opener, closer or quote character.
 *
 * @param character Character of the key in the test text.
 * @param key       Pair key; not changed for a character of no pair.
 */
static void find_pair_key(char character, pair_key_t *key) {
    for (size_t index = 0; index < PAIR_KIND_COUNT; index++) {
        enum pair_kind kind      = (enum pair_kind)index;
        bool           is_quote  = kind >= FIRST_QUOTE_KIND;
        bool           is_opener = pair_characters[kind].opener == character;
        bool           is_closer = pair_characters[kind].closer == character;
        if (is_quote && is_opener) {
            *key = (pair_key_t){.key_class = PAIR_KEY_QUOTE, .kind = kind};
        } else if (!is_quote && is_opener) {
            *key = (pair_key_t){.key_class = PAIR_KEY_OPENER, .kind = kind};
        } else if (!is_quote && is_closer) {
            *key = (pair_key_t){.key_class = PAIR_KEY_CLOSER, .kind = kind};
        }
    }
}

/**
 * Gives the pair key of a character of a test text.
 *
 * @param character Bracket or quote, a character from key_characters, or a
 *                  character that a key types.
 * @return Pair key of the key that the operator pushes.
 */
static pair_key_t key_of_character(char character) {
    pair_key_t key = {.key_class = PAIR_KEY_CHARACTER};
    for (size_t index = 0; index < KEY_CHARACTER_COUNT; index++) {
        if (key_characters[index].character == character) {
            key.key_class = key_characters[index].key_class;
        }
    }
    find_pair_key(character, &key);
    return key;
}

/**
 * Pushes one key and applies the result to the editor model.
 *
 * @param typing    Stack, editor model and clock.
 * @param character Character of the key in the test text.
 * @return Result of the push.
 */
static pair_result_t type_key(typing_t *typing, char character) {
    pair_result_t result =
        pair_stack_push(&typing->stack, key_of_character(character), typing->now);
    if (result.passes) {
        apply_passed_key(&typing->editor, character);
    }
    // Outputs go one time, then one more time for each repeat
    for (size_t send = 0; send <= result.repeat_count; send++) {
        for (size_t index = 0; index < result.output_count; index++) {
            apply_output(&typing->editor, &result, index);
        }
    }
    return result;
}

/**
 * Pushes the key of each character of a text, and applies each result.
 *
 * @param typing Stack, editor model and clock.
 * @param text   Characters of the keys, in push order.
 */
static void type_keys(typing_t *typing, const char *text) {
    for (size_t index = 0; text[index] != '\0'; index++) {
        type_key(typing, text[index]);
    }
}

/**
 * Fails the test if the editor model does not show a text with its cursor.
 *
 * @param typing   Stack, editor model and clock.
 * @param expected Text with a '|' at the place of the cursor.
 */
static void assert_editor_shows(const typing_t *typing, const char *expected) {
    char        shown[EDITOR_CAPACITY + ONE_CHARACTER];
    const char *text   = typing->editor.text;
    size_t      cursor = typing->editor.cursor;
    snprintf(shown, sizeof(shown), "%.*s|%s", (int)cursor, text, &text[cursor]);
    assert_string_equal(shown, expected);
}

/**
 * Gives the top entry of the stack. Fails the test if the stack is empty.
 *
 * @param typing Stack, editor model and clock.
 * @return Top entry.
 */
static const pair_entry_t *top_entry(const typing_t *typing) {
    assert_true(typing->stack.depth > 0);
    // Bottom entry for an empty stack, so a failed case reads no memory outside
    size_t depth = typing->stack.depth;
    if (depth == 0) {
        return &typing->stack.entries[0];
    }
    return &typing->stack.entries[depth - ONE_ENTRY];
}

/**
 * Counts a key that an earlier handler sent.
 *
 * @param typing Stack, editor model and clock.
 * @param sent   Key class of the sent key.
 */
static void count_sent_key(typing_t *typing, enum pair_key_class sent) {
    pair_key_t sent_key = {.key_class = sent};
    pair_stack_count_sent_key(&typing->stack, sent_key, typing->now);
}

static void bracket_pairs_logic_r1_opener_sends_pair_and_left(void **state) {
    (void)state;
    static const char *const shown[] = {"(|)", "[|]", "{|}", "<|>"};
    for (size_t kind = 0; kind < FIRST_QUOTE_KIND; kind++) {
        typing_t      typing = {0};
        pair_result_t result = type_key(&typing, pair_characters[kind].opener);
        assert_false(result.passes);
        assert_int_equal(result.kind, kind);
        assert_int_equal(result.output_count, 3);
        assert_int_equal(result.outputs[0], PAIR_OUTPUT_OPENER);
        assert_int_equal(result.outputs[1], PAIR_OUTPUT_CLOSER);
        assert_int_equal(result.outputs[2], PAIR_OUTPUT_LEFT);
        assert_editor_shows(&typing, shown[kind]);
    }
}

static void bracket_pairs_logic_r2_opener_puts_entry_with_count_zero(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{");
    assert_int_equal(typing.stack.depth, 1);
    assert_int_equal(top_entry(&typing)->kind, PAIR_CURLY_BRACES);
    assert_int_equal(top_entry(&typing)->count, 0);
    assert_int_equal(top_entry(&typing)->offset, 0);
    type_keys(&typing, "(");
    assert_int_equal(typing.stack.depth, 2);
    assert_int_equal(top_entry(&typing)->kind, PAIR_PARENTHESES);
    assert_int_equal(top_entry(&typing)->count, 0);
    assert_int_equal(top_entry(&typing)->offset, 0);
}

static void bracket_pairs_logic_r4_character_adds_one_to_top_count(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{a(bc");
    assert_editor_shows(&typing, "{a(bc|)}");
    assert_int_equal(top_entry(&typing)->count, 2);
    assert_int_equal(typing.stack.entries[0].count, 1);
    // A modifier types nothing, so the count stays
    type_keys(&typing, "^");
    assert_int_equal(top_entry(&typing)->count, 2);
}

static void bracket_pairs_logic_r4_no_count_while_stack_is_empty(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "ab");
    assert_editor_shows(&typing, "ab|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r4_sent_character_adds_one(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    count_sent_key(&typing, PAIR_KEY_CHARACTER);
    assert_int_equal(top_entry(&typing)->count, 1);
    // A key that types nothing leaves the count
    count_sent_key(&typing, PAIR_KEY_NO_CHARACTER);
    assert_int_equal(top_entry(&typing)->count, 1);
}

static void bracket_pairs_logic_r4_sent_backspace_subtracts_one(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(ab");
    count_sent_key(&typing, PAIR_KEY_BACKSPACE);
    assert_int_equal(top_entry(&typing)->count, 1);
    count_sent_key(&typing, PAIR_KEY_BACKSPACE);
    assert_int_equal(typing.stack.depth, 2);
    // Sent Backspace deleted the opener, so the pair is gone
    count_sent_key(&typing, PAIR_KEY_BACKSPACE);
    assert_int_equal(typing.stack.depth, 1);
}

static void bracket_pairs_logic_r4_sent_key_with_empty_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    count_sent_key(&typing, PAIR_KEY_CHARACTER);
    count_sent_key(&typing, PAIR_KEY_BACKSPACE);
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r12_sent_key_after_idle_time(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    typing.now += PAIR_IDLE_TIME;
    count_sent_key(&typing, PAIR_KEY_CHARACTER);
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r5_ac2_tab_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(x");
    pair_result_t result = type_key(&typing, '\t');
    assert_false(result.passes);
    assert_int_equal(result.output_count, 1);
    assert_int_equal(result.outputs[0], PAIR_OUTPUT_RIGHT);
    assert_editor_shows(&typing, "(x)|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r7_ac4_jump_out_adds_count_to_new_top(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(a\t");
    assert_editor_shows(&typing, "{(a)|}");
    assert_int_equal(typing.stack.depth, 1);
    // Removed count 1, plus the two characters of the pair
    assert_int_equal(top_entry(&typing)->count, 3);
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "{(a)}|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r6_ac3_matching_closer_jumps_out(void **state) {
    (void)state;
    static const char *const shown[] = {"(x)|", "[x]|", "{x}|", "<x>|"};
    for (size_t kind = 0; kind < FIRST_QUOTE_KIND; kind++) {
        typing_t typing = {0};
        type_key(&typing, pair_characters[kind].opener);
        type_keys(&typing, "x");
        pair_result_t result = type_key(&typing, pair_characters[kind].closer);
        assert_false(result.passes);
        assert_int_equal(result.output_count, 1);
        assert_int_equal(result.outputs[0], PAIR_OUTPUT_RIGHT);
        assert_editor_shows(&typing, shown[kind]);
        assert_int_equal(typing.stack.depth, 0);
    }
}

static void bracket_pairs_logic_r7_closer_adds_count_to_new_top(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "[(ab)");
    assert_editor_shows(&typing, "[(ab)|]");
    assert_int_equal(top_entry(&typing)->count, 4);
}

static void bracket_pairs_logic_r8_backspace_subtracts_one(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(ab");
    pair_result_t result = type_key(&typing, '\b');
    assert_true(result.passes);
    assert_int_equal(result.output_count, 0);
    assert_editor_shows(&typing, "(a|)");
    assert_int_equal(top_entry(&typing)->count, 1);
}

static void bracket_pairs_logic_r9_ac5_backspace_at_zero_deletes_pair(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(ab\b\b");
    pair_result_t result = type_key(&typing, '\b');
    assert_false(result.passes);
    assert_int_equal(result.output_count, 2);
    assert_int_equal(result.outputs[0], PAIR_OUTPUT_BACKSPACE);
    assert_int_equal(result.outputs[1], PAIR_OUTPUT_DELETE);
    assert_editor_shows(&typing, "|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r9_inner_pair_delete_keeps_outer_count(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{a(\b");
    assert_editor_shows(&typing, "{a|}");
    assert_int_equal(typing.stack.depth, 1);
    assert_int_equal(top_entry(&typing)->count, 1);
}

static void bracket_pairs_logic_r10_ac7_clearing_key_clears_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(x");
    pair_result_t result = type_key(&typing, ENTER_CHARACTER);
    assert_true(result.passes);
    assert_int_equal(typing.stack.depth, 0);
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "{(x\n\t|)}");
}

static void bracket_pairs_logic_r11_ac8_other_closer_counts_as_character(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(");
    pair_result_t result = type_key(&typing, ']');
    assert_true(result.passes);
    assert_editor_shows(&typing, "{(]|)}");
    assert_int_equal(typing.stack.depth, 2);
    assert_int_equal(top_entry(&typing)->count, 1);
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "{(])|}");
}

static void bracket_pairs_logic_r11_ac20_operator_in_pair(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(p->x)");
    assert_editor_shows(&typing, "(p->x)|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r12_ac9_idle_time_clears_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    typing.now += PAIR_IDLE_TIME;
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "(\t|)");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r12_push_before_idle_time_keeps_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    typing.now += JUST_BEFORE_IDLE_TIME;
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "()|");
}

static void bracket_pairs_logic_r12_each_push_starts_idle_time_again(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    typing.now += JUST_BEFORE_IDLE_TIME;
    // A modifier types nothing, but it is a key push
    type_keys(&typing, "^");
    typing.now += JUST_BEFORE_IDLE_TIME;
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "()|");
}

static void bracket_pairs_logic_r13_ac11_empty_stack_keys_pass(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "\t)]}>x\b");
    assert_editor_shows(&typing, "\t)]}>|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r14_ninth_pair_removes_oldest(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "[((((((((");
    assert_int_equal(typing.stack.depth, PAIR_STACK_CAPACITY);
    for (size_t index = 0; index < PAIR_STACK_CAPACITY; index++) {
        assert_int_equal(typing.stack.entries[index].kind, PAIR_PARENTHESES);
    }
    type_keys(&typing, "\t\t\t\t\t\t\t\t");
    assert_int_equal(typing.stack.depth, 0);
    // Stack has no entry for the oldest pair, so its closer passes
    assert_true(type_key(&typing, ']').passes);
}

static void bracket_pairs_logic_r16_ac12_quote_sends_pair_and_left(void **state) {
    (void)state;
    static const char *const shown[] = {"\"|\"", "`|`"};
    for (size_t kind = FIRST_QUOTE_KIND; kind < PAIR_KIND_COUNT; kind++) {
        typing_t      typing = {0};
        pair_result_t result = type_key(&typing, pair_characters[kind].opener);
        assert_false(result.passes);
        assert_int_equal(result.kind, kind);
        assert_editor_shows(&typing, shown[kind - FIRST_QUOTE_KIND]);
        assert_int_equal(typing.stack.depth, 1);
        assert_int_equal(top_entry(&typing)->kind, kind);
        assert_int_equal(top_entry(&typing)->count, 0);
    }
}

static void bracket_pairs_logic_r16_ac15_quote_in_bracket_pair(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(\"");
    assert_editor_shows(&typing, "(\"|\")");
    assert_int_equal(typing.stack.depth, 2);
}

static void bracket_pairs_logic_r16_other_quote_opens_pair(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "\"`");
    assert_editor_shows(&typing, "\"`|`\"");
    assert_int_equal(typing.stack.depth, 2);
}

static void bracket_pairs_logic_r17_ac13_same_quote_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "\"ab");
    pair_result_t result = type_key(&typing, '"');
    assert_false(result.passes);
    assert_int_equal(result.output_count, 1);
    assert_int_equal(result.outputs[0], PAIR_OUTPUT_RIGHT);
    assert_editor_shows(&typing, "\"ab\"|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r17_ac14_backtick_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "`1");
    assert_editor_shows(&typing, "`1|`");
    type_keys(&typing, "`");
    assert_editor_shows(&typing, "`1`|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r17_quote_adds_count_to_new_top(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(\"a\"");
    assert_int_equal(top_entry(&typing)->count, 3);
    type_keys(&typing, "\b\b\b\b");
    assert_editor_shows(&typing, "|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r19_left_in_pair_adds_one_to_offset(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(abc");
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, LEFT_CHARACTER);
    assert_true(result.passes);
    assert_editor_shows(&typing, "(a|bc)");
    assert_int_equal(top_entry(&typing)->count, 3);
    assert_int_equal(top_entry(&typing)->offset, 2);
}

static void bracket_pairs_logic_r20_left_at_opener_clears_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(a");
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, LEFT_CHARACTER);
    assert_true(result.passes);
    assert_editor_shows(&typing, "{|(a)}");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r21_right_subtracts_one_from_offset(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(abc");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, RIGHT_CHARACTER);
    assert_true(result.passes);
    assert_editor_shows(&typing, "(ab|c)");
    assert_int_equal(top_entry(&typing)->offset, 1);
}

static void bracket_pairs_logic_r22_r7_right_at_closer_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(a");
    pair_result_t result = type_key(&typing, RIGHT_CHARACTER);
    assert_true(result.passes);
    assert_int_equal(result.output_count, 0);
    assert_editor_shows(&typing, "{(a)|}");
    assert_int_equal(typing.stack.depth, 1);
    assert_int_equal(top_entry(&typing)->count, 3);
    assert_int_equal(top_entry(&typing)->offset, 0);
}

static void bracket_pairs_logic_r13_left_and_right_pass_with_empty_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "ab");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, RIGHT_CHARACTER);
    assert_editor_shows(&typing, "a|b");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r4_character_after_left_keeps_offset(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(ac");
    type_key(&typing, LEFT_CHARACTER);
    type_keys(&typing, "b");
    assert_editor_shows(&typing, "(ab|c)");
    assert_int_equal(top_entry(&typing)->count, 3);
    assert_int_equal(top_entry(&typing)->offset, 1);
}

static void bracket_pairs_logic_r5_ac17_tab_after_left_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(abc");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, '\t');
    assert_false(result.passes);
    assert_int_equal(result.output_count, 1);
    assert_int_equal(result.outputs[0], PAIR_OUTPUT_RIGHT);
    assert_int_equal(result.repeat_count, 2);
    assert_editor_shows(&typing, "(abc)|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r5_ac18_tab_after_insert_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(ac");
    type_key(&typing, LEFT_CHARACTER);
    type_keys(&typing, "b\t");
    assert_editor_shows(&typing, "(abc)|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r6_ac16_closer_after_left_jumps_out(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(abc");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    type_keys(&typing, ")");
    assert_editor_shows(&typing, "(abc)|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r17_quote_after_left_jumps_past_quote(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "\"ab");
    type_key(&typing, LEFT_CHARACTER);
    type_keys(&typing, "\"");
    assert_editor_shows(&typing, "\"ab\"|");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r7_jump_out_keeps_offset_of_new_top(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{x");
    type_key(&typing, LEFT_CHARACTER);
    type_keys(&typing, "(a\t");
    assert_editor_shows(&typing, "{(a)|x}");
    assert_int_equal(top_entry(&typing)->count, 4);
    assert_int_equal(top_entry(&typing)->offset, 1);
    type_keys(&typing, "\t");
    assert_editor_shows(&typing, "{(a)x}|");
}

static void bracket_pairs_logic_r8_backspace_after_left_subtracts_one(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(abc");
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, '\b');
    assert_true(result.passes);
    assert_editor_shows(&typing, "(a|c)");
    assert_int_equal(top_entry(&typing)->count, 2);
    assert_int_equal(top_entry(&typing)->offset, 1);
}

static void bracket_pairs_logic_r23_backspace_at_opener_clears_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(ab");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    pair_result_t result = type_key(&typing, '\b');
    assert_true(result.passes);
    assert_int_equal(result.output_count, 0);
    assert_editor_shows(&typing, "{|ab)}");
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r23_sent_backspace_at_opener_clears(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "{(ab");
    type_key(&typing, LEFT_CHARACTER);
    type_key(&typing, LEFT_CHARACTER);
    count_sent_key(&typing, PAIR_KEY_BACKSPACE);
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r24_long_release_clears_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(a");
    pair_stack_release_passed_key(&typing.stack, PAIR_REPEAT_SAFE_TIME);
    assert_int_equal(typing.stack.depth, 0);
}

static void bracket_pairs_logic_r24_short_release_keeps_stack(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(a");
    pair_stack_release_passed_key(&typing.stack, JUST_BEFORE_REPEAT_SAFE_TIME);
    assert_int_equal(typing.stack.depth, 1);
    assert_int_equal(top_entry(&typing)->count, 1);
}

static void bracket_pairs_logic_r25_has_pairs_while_stack_is_not_empty(void **state) {
    (void)state;
    typing_t typing = {0};
    assert_false(pair_stack_has_pairs(&typing.stack, typing.now));
    type_keys(&typing, "(");
    assert_true(pair_stack_has_pairs(&typing.stack, typing.now));
    type_keys(&typing, "\t");
    assert_false(pair_stack_has_pairs(&typing.stack, typing.now));
}

static void bracket_pairs_logic_r25_r12_no_pairs_after_idle_time(void **state) {
    (void)state;
    typing_t typing = {0};
    type_keys(&typing, "(");
    typing.now += JUST_BEFORE_IDLE_TIME;
    assert_true(pair_stack_has_pairs(&typing.stack, typing.now));
    typing.now++;
    assert_false(pair_stack_has_pairs(&typing.stack, typing.now));
}

// #endregion

// #region Test runner

/** Cases of this program, in the order of the spec requirements */
static const struct CMUnitTest bracket_pairs_logic_cases[] = {
    cmocka_unit_test(bracket_pairs_logic_r1_opener_sends_pair_and_left),
    cmocka_unit_test(bracket_pairs_logic_r2_opener_puts_entry_with_count_zero),
    cmocka_unit_test(bracket_pairs_logic_r4_character_adds_one_to_top_count),
    cmocka_unit_test(bracket_pairs_logic_r4_no_count_while_stack_is_empty),
    cmocka_unit_test(bracket_pairs_logic_r4_sent_character_adds_one),
    cmocka_unit_test(bracket_pairs_logic_r4_sent_backspace_subtracts_one),
    cmocka_unit_test(bracket_pairs_logic_r4_sent_key_with_empty_stack),
    cmocka_unit_test(bracket_pairs_logic_r5_ac2_tab_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r7_ac4_jump_out_adds_count_to_new_top),
    cmocka_unit_test(bracket_pairs_logic_r6_ac3_matching_closer_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r7_closer_adds_count_to_new_top),
    cmocka_unit_test(bracket_pairs_logic_r8_backspace_subtracts_one),
    cmocka_unit_test(bracket_pairs_logic_r9_ac5_backspace_at_zero_deletes_pair),
    cmocka_unit_test(bracket_pairs_logic_r9_inner_pair_delete_keeps_outer_count),
    cmocka_unit_test(bracket_pairs_logic_r10_ac7_clearing_key_clears_stack),
    cmocka_unit_test(bracket_pairs_logic_r11_ac8_other_closer_counts_as_character),
    cmocka_unit_test(bracket_pairs_logic_r11_ac20_operator_in_pair),
    cmocka_unit_test(bracket_pairs_logic_r12_ac9_idle_time_clears_stack),
    cmocka_unit_test(bracket_pairs_logic_r12_push_before_idle_time_keeps_stack),
    cmocka_unit_test(bracket_pairs_logic_r12_each_push_starts_idle_time_again),
    cmocka_unit_test(bracket_pairs_logic_r12_sent_key_after_idle_time),
    cmocka_unit_test(bracket_pairs_logic_r13_ac11_empty_stack_keys_pass),
    cmocka_unit_test(bracket_pairs_logic_r14_ninth_pair_removes_oldest),
    cmocka_unit_test(bracket_pairs_logic_r16_ac12_quote_sends_pair_and_left),
    cmocka_unit_test(bracket_pairs_logic_r16_ac15_quote_in_bracket_pair),
    cmocka_unit_test(bracket_pairs_logic_r16_other_quote_opens_pair),
    cmocka_unit_test(bracket_pairs_logic_r17_ac13_same_quote_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r17_ac14_backtick_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r17_quote_adds_count_to_new_top),
    cmocka_unit_test(bracket_pairs_logic_r19_left_in_pair_adds_one_to_offset),
    cmocka_unit_test(bracket_pairs_logic_r20_left_at_opener_clears_stack),
    cmocka_unit_test(bracket_pairs_logic_r21_right_subtracts_one_from_offset),
    cmocka_unit_test(bracket_pairs_logic_r22_r7_right_at_closer_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r13_left_and_right_pass_with_empty_stack),
    cmocka_unit_test(bracket_pairs_logic_r4_character_after_left_keeps_offset),
    cmocka_unit_test(bracket_pairs_logic_r5_ac17_tab_after_left_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r5_ac18_tab_after_insert_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r6_ac16_closer_after_left_jumps_out),
    cmocka_unit_test(bracket_pairs_logic_r17_quote_after_left_jumps_past_quote),
    cmocka_unit_test(bracket_pairs_logic_r7_jump_out_keeps_offset_of_new_top),
    cmocka_unit_test(bracket_pairs_logic_r8_backspace_after_left_subtracts_one),
    cmocka_unit_test(bracket_pairs_logic_r23_backspace_at_opener_clears_stack),
    cmocka_unit_test(bracket_pairs_logic_r23_sent_backspace_at_opener_clears),
    cmocka_unit_test(bracket_pairs_logic_r24_long_release_clears_stack),
    cmocka_unit_test(bracket_pairs_logic_r24_short_release_keeps_stack),
    cmocka_unit_test(bracket_pairs_logic_r25_has_pairs_while_stack_is_not_empty),
    cmocka_unit_test(bracket_pairs_logic_r25_r12_no_pairs_after_idle_time),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(bracket_pairs_logic_cases, NULL, NULL);
}

// #endregion
