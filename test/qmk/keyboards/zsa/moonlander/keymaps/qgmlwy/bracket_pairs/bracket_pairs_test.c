/** @file
 * Tests of the bracket and quote pairs through the keymap's key event hook, with
 * the recording fake of QMK.
 *
 * Each key comes from its spec position in the keymap. When the hook lets a key
 * pass, QMK sends that key. Each case models this with a tap of the key in the call
 * log, so the log is all that the host gets, in order. The symbol holds register
 * and unregister their symbols themselves.
 */

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "keycodes/keycodes.h"
#include "keymap_introspection.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Tap count of a key that is not a tap-hold key, or of a tap-hold hold */
#define NO_TAP 0

/** Tap count of the tap of a tap-hold key */
#define ONE_TAP 1

/** Value of push_and_release() is_tap for a key that is not a tap-hold key, or a hold
 */
#define NOT_A_TAP false

/** Value of push_and_release() is_tap for the tap of a tap-hold key */
#define A_TAP true

/** First grid column of the right half */
#define FIRST_RIGHT_COLUMN 7

/** Matrix rows from a left half row to the same row of the right half */
#define RIGHT_HALF_ROW_OFFSET RIGHT_NUMBER_ROW

/** Left Shift bit, as the hold of the N home row key sets it */
#define LEFT_SHIFT_BIT MOD_BIT(KC_LEFT_SHIFT)

/** Right Shift bit, as the hold of the A home row key sets it */
#define RIGHT_SHIFT_BIT MOD_BIT(KC_RIGHT_SHIFT)

/**
 * Gives the keycode at a grid position of the spec.
 *
 * @param layer  Layer of the key.
 * @param row    Grid row, 0 to 4 from the number row down.
 * @param column Grid column, 0 to 13 from left to right.
 * @return Keycode at that position.
 */
static uint16_t key_at(uint8_t layer, uint8_t row, uint8_t column) {
    if (column < FIRST_RIGHT_COLUMN) {
        return keycode_at_keymap_location(layer, row, column);
    }
    uint8_t matrix_row    = (uint8_t)(row + RIGHT_HALF_ROW_OFFSET);
    uint8_t matrix_column = (uint8_t)(column - FIRST_RIGHT_COLUMN);
    return keycode_at_keymap_location(layer, matrix_row, matrix_column);
}

/** Key position of the spec */
typedef struct {
    uint8_t layer;  /**< Layer of the key */
    uint8_t row;    /**< Grid row */
    uint8_t column; /**< Grid column */
} grid_position_t;

/** Tab key (1,0) */
static const grid_position_t tab_key = {
    .layer  = LAYER_BASE,
    .row    = 1,
    .column = 0,
};

/** N (2,4), the left home row Shift */
static const grid_position_t left_shift_key = {
    .layer  = LAYER_BASE,
    .row    = 2,
    .column = 4,
};

/** A (2,9), the right home row Shift */
static const grid_position_t right_shift_key = {
    .layer  = LAYER_BASE,
    .row    = 2,
    .column = 9,
};

/** Quote key (2,13) */
static const grid_position_t quote_key = {
    .layer  = LAYER_BASE,
    .row    = 2,
    .column = 13,
};

/** Z (3,1), the "<" hold */
static const grid_position_t opening_angle_bracket_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 1,
};

/** X (3,2), the "[" hold */
static const grid_position_t opening_square_bracket_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 2,
};

/** C (3,3), the "{" hold */
static const grid_position_t opening_curly_brace_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 3,
};

/** V (3,4), the "(" hold */
static const grid_position_t opening_parenthesis_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 4,
};

/** J (3,5), the "_" hold */
static const grid_position_t underscore_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 5,
};

/** P (3,9), the ")" hold */
static const grid_position_t closing_parenthesis_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 9,
};

/** Period (3,11), the "]" hold */
static const grid_position_t closing_square_bracket_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 11,
};

/** Mouse right key (4,12) */
static const grid_position_t mouse_right_key = {
    .layer  = LAYER_BASE,
    .row    = 4,
    .column = 12,
};

/** Backtick (1,5) of the special mode */
static const grid_position_t backtick_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 1,
    .column = 5,
};

/** Digit 1 (3,9) of the special mode */
static const grid_position_t special_digit_one_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 3,
    .column = 9,
};

/** Time in ms of the wait of AC9, longer than the idle time */
#define LONGER_THAN_IDLE_TIME 31000

/** Backspace pushes that empty the pair after snake_case "a b" */
#define UNDERSCORE_CASE_BACKSPACES 4

/** Backspace pushes that empty the pair after camelCase "a b" */
#define CAPITAL_CASE_BACKSPACES 3

/**
 * Gives the keycode at a key position of the spec.
 *
 * @param position Layer and grid position of the key.
 * @return Keycode at that position.
 */
static uint16_t keycode_of(grid_position_t position) {
    return key_at(position.layer, position.row, position.column);
}

/**
 * Gives the keycode at a grid position of the base layer.
 *
 * @param row    Grid row.
 * @param column Grid column.
 * @return Keycode at that position.
 */
static uint16_t base_key_at(uint8_t row, uint8_t column) {
    return key_at(LAYER_BASE, row, column);
}

/**
 * Pushes and releases one key through process_record_user().
 *
 * @param keycode Keycode that QMK gives the hook.
 * @param is_tap  A_TAP for the tap of a tap-hold key, NOT_A_TAP else.
 * @return Hook result for the push.
 */
static bool push_and_release(uint16_t keycode, bool is_tap) {
    keyrecord_t push    = {.event = {.pressed = true}};
    keyrecord_t release = {.event = {.pressed = false}};
    // QMK gives both events of a tap the same tap count
    push.tap.count    = is_tap ? ONE_TAP : NO_TAP;
    release.tap.count = push.tap.count;
    bool passes       = process_record_user(keycode, &push);
    process_record_user(keycode, &release);
    return passes;
}

/**
 * Sends one key event of a key that is not a tap-hold key, or of a hold.
 *
 * @param keycode Keycode that QMK gives the hook.
 * @param pressed True for a push, false for a release.
 * @return Hook result.
 */
static bool send_key_event(uint16_t keycode, bool pressed) {
    keyrecord_t record = {.event = {.pressed = pressed}};
    return process_record_user(keycode, &record);
}

/**
 * Holds a key past its tapping term, then releases it.
 *
 * @param keycode Keycode of a tap-hold key.
 */
static void hold_key(uint16_t keycode) {
    // Symbol holds send their own symbol, so a hold never goes on to QMK
    assert_false(push_and_release(keycode, NOT_A_TAP));
}

/**
 * Pushes and releases a key that is not a tap-hold key. A key that passes goes to
 * the call log as a tap, as QMK sends it.
 *
 * @param keycode Keycode of the key.
 */
static void type_key(uint16_t keycode) {
    if (push_and_release(keycode, NOT_A_TAP)) {
        tap_code16(keycode);
    }
}

/**
 * Taps a tap-hold key. The tap key goes to the call log, as QMK sends it.
 *
 * @param keycode Keycode of a mod-tap or layer-tap key.
 */
static void tap_tap_hold_key(uint16_t keycode) {
    if (push_and_release(keycode, A_TAP)) {
        tap_code16(QK_MOD_TAP_GET_TAP_KEYCODE(keycode));
    }
}

/**
 * Pushes and releases a case mode key. Case modes take the push and let the release
 * pass.
 *
 * @param keycode Keycode of the mode key.
 */
static void push_mode_key(uint16_t keycode) {
    keyrecord_t push    = {.event = {.pressed = true}};
    keyrecord_t release = {.event = {.pressed = false}};
    assert_false(process_record_user(keycode, &push));
    assert_true(process_record_user(keycode, &release));
}

/** Clears the fake, a pause, the pair stack and any case mode of an earlier case */
static void start_case(void) {
    reset_fake_qmk();
    bracket_pairs_set_paused(false);
    bracket_pairs_clear();
    case_modes_end_all();
}

/** Bracket hold of the spec and the symbols of its pair */
typedef struct {
    uint8_t  row;    /**< Grid row of the opener hold */
    uint8_t  column; /**< Grid column of the opener hold */
    uint16_t opener; /**< Symbol of the opener */
    uint16_t closer; /**< Symbol of the matching closer */
} bracket_pair_t;

/** Opener holds of the spec, with the grid position of each one */
static const bracket_pair_t bracket_pairs[] = {
    {
        .row    = 3,
        .column = 4,
        .opener = KC_LEFT_PAREN,
        .closer = KC_RIGHT_PAREN,
    },
    {
        .row    = 3,
        .column = 2,
        .opener = KC_LEFT_BRACKET,
        .closer = KC_RIGHT_BRACKET,
    },
    {
        .row    = 3,
        .column = 3,
        .opener = KC_LEFT_CURLY_BRACE,
        .closer = KC_RIGHT_CURLY_BRACE,
    },
    {
        .row    = 3,
        .column = 1,
        .opener = KC_LEFT_ANGLE_BRACKET,
        .closer = KC_RIGHT_ANGLE_BRACKET,
    },
};

/** Closer holds of the spec, in the order of bracket_pairs */
static const uint8_t closer_columns[] = {9, 11, 10, 12};

/** Grid row of the bracket holds */
#define BRACKET_ROW 3

/** Calls of a hold of V, the "(" hold */
#define PARENTHESES_PAIR_CALLS                                                 \
    {CALLED_TAP_CODE16, KC_LEFT_PAREN}, {CALLED_TAP_CODE16, KC_RIGHT_PAREN}, { \
        CALLED_TAP_CODE16, KC_LEFT                                             \
    }

static void bracket_pairs_r1_ac1_ac10_opener_hold_sends_pair(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(bracket_pairs); index++) {
        const bracket_pair_t *pair = &bracket_pairs[index];
        start_case();
        hold_key(base_key_at(pair->row, pair->column));
        // Release of the hold sends nothing, because QMK did not get the push
        const qmk_call_t expected[] = {
            {CALLED_TAP_CODE16, pair->opener},
            {CALLED_TAP_CODE16, pair->closer},
            {CALLED_TAP_CODE16, KC_LEFT},
        };
        assert_recorded_calls(expected, ARRAY_SIZE(expected));
    }
}

static void bracket_pairs_release_of_taken_push_stops(void **state) {
    (void)state;
    start_case();
    // Push of the "(" hold and of the jump-out Tab: QMK must not get the releases
    assert_false(send_key_event(keycode_of(opening_parenthesis_key), true));
    assert_false(send_key_event(KC_TAB, true));
    assert_false(send_key_event(KC_TAB, false));
    assert_false(send_key_event(keycode_of(opening_parenthesis_key), false));
    // Stack is empty, so Tab passes; then its release passes too
    assert_true(send_key_event(KC_TAB, true));
    assert_true(send_key_event(KC_TAB, false));
}

static void bracket_pairs_r3_ac6_shift_opener_sends_only_opener(void **state) {
    (void)state;
    start_case();
    // Hold of N, the left home row Shift, then the "<" hold on Z
    assert_true(push_and_release(keycode_of(left_shift_key), NOT_A_TAP));
    set_fake_modifiers(LEFT_SHIFT_BIT);
    hold_key(keycode_of(opening_angle_bracket_key));
    set_fake_modifiers(0);
    // Stack stays empty, so Tab passes
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        {CALLED_DEL_MODS, LEFT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_LEFT_ANGLE_BRACKET},
        {CALLED_ADD_MODS, LEFT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r3_shift_square_bracket_sends_a_square_bracket(void **state) {
    (void)state;
    start_case();
    // With the real Shift held, "[" would reach the host as "{", so the module
    // sends the opener with the Shift off
    set_fake_modifiers(LEFT_SHIFT_BIT);
    hold_key(keycode_of(opening_square_bracket_key));
    set_fake_modifiers(0);
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        {CALLED_DEL_MODS, LEFT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_LEFT_BRACKET},
        {CALLED_ADD_MODS, LEFT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_each_character_counts(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Tap of the Z symbol hold, then the "_" hold on J: two characters
    tap_tap_hold_key(keycode_of(opening_angle_bracket_key));
    hold_key(keycode_of(underscore_key));
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_Z},
        {CALLED_REGISTER_CODE16, KC_UNDERSCORE},
        {CALLED_UNREGISTER_CODE16, KC_UNDERSCORE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_case_mode_underscore_counts(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    push_mode_key(SNAKE_CASE_KEY);
    // Case modes send the underscore for the Space themselves
    type_key(KC_A);
    type_key(KC_SPACE);
    type_key(KC_B);
    // Three characters, then the empty pair
    for (size_t index = 0; index < UNDERSCORE_CASE_BACKSPACES; index++) {
        type_key(KC_BACKSPACE);
    }
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_UNDERSCORE},
        {CALLED_TAP_CODE16, KC_B},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_case_mode_capital_counts(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    push_mode_key(CAMEL_CASE_KEY);
    // Space types nothing; case modes send the capital B themselves
    type_key(KC_A);
    type_key(KC_SPACE);
    type_key(KC_B);
    // Two characters, then the empty pair
    for (size_t index = 0; index < CAPITAL_CASE_BACKSPACES; index++) {
        type_key(KC_BACKSPACE);
    }
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, LSFT(KC_B)},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_sent_backspace_counts(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_B);
    // An earlier handler sends a Backspace in place of a push
    tap_code16(KC_BACKSPACE);
    bracket_pairs_count_sent_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_B},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r5_ac2_tab_sends_right(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_X);
    type_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_X},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r6_ac3_matching_closer_sends_right(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(bracket_pairs); index++) {
        const bracket_pair_t *pair = &bracket_pairs[index];
        start_case();
        hold_key(base_key_at(pair->row, pair->column));
        type_key(KC_X);
        hold_key(base_key_at(BRACKET_ROW, closer_columns[index]));
        const qmk_call_t expected[] = {
            {CALLED_TAP_CODE16, pair->opener}, {CALLED_TAP_CODE16, pair->closer},
            {CALLED_TAP_CODE16, KC_LEFT},      {CALLED_TAP_CODE16, KC_X},
            {CALLED_TAP_CODE16, KC_RIGHT},
        };
        assert_recorded_calls(expected, ARRAY_SIZE(expected));
    }
}

static void bracket_pairs_r7_ac4_second_tab_leaves_outer_pair(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_curly_brace_key));
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_TAB);
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_LEFT_CURLY_BRACE},
        {CALLED_TAP_CODE16, KC_RIGHT_CURLY_BRACE},
        {CALLED_TAP_CODE16, KC_LEFT},
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r8_r9_ac5_backspace_empties_line(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_B);
    // Backspace key L2
    uint16_t backspace =
        keycode_at_keymap_location(LAYER_BASE, LEFT_THUMB_KEYS, LEFT_MIDDLE_THUMB);
    type_key(backspace);
    type_key(backspace);
    type_key(backspace);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_B},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r9_outputs_with_no_shift(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Shift with Delete is a cut on Windows, so the outputs go without Shift
    set_fake_modifiers(LEFT_SHIFT_BIT);
    type_key(KC_BACKSPACE);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_DEL_MODS, LEFT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
        {CALLED_ADD_MODS, LEFT_SHIFT_BIT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/** Keys of spec 33 R10 that clear the pair stack, as the keymap has them */
static const uint16_t clearing_keys[] = {
    KC_LEFT,
    KC_RIGHT,
    KC_UP,
    KC_DOWN,
    KC_HOME,
    KC_END,
    KC_ENTER,
    KC_ESCAPE,
    KC_DELETE,
    KC_F1,
    KC_F12,
    QK_MOUSE_CURSOR_LEFT,
    QK_MOUSE_CURSOR_RIGHT,
    QK_MOUSE_BUTTON_1,
    QK_MOUSE_BUTTON_3,
    QK_MOUSE_WHEEL_UP,
    QK_MOUSE_WHEEL_DOWN,
    QK_MOUSE_ACCELERATION_0,
};

static void bracket_pairs_r10_clearing_key_clears_stack(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(clearing_keys); index++) {
        start_case();
        hold_key(keycode_of(opening_parenthesis_key));
        // Precision key handler takes its key, so only the Tab result counts
        push_and_release(clearing_keys[index], NOT_A_TAP);
        assert_true(push_and_release(KC_TAB, NOT_A_TAP));
    }
}

static void bracket_pairs_r10_ac7_mouse_key_then_tab(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_X);
    // Mouse right key; QMK moves the cursor and sends no key
    assert_true(push_and_release(keycode_of(mouse_right_key), NOT_A_TAP));
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_X},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/** Modifier bits of the Ctrl, Alt and GUI home row holds */
static const uint8_t shortcut_modifiers[] = {
    MOD_BIT(KC_LEFT_CTRL),
    MOD_BIT(KC_LEFT_ALT),
    MOD_BIT(KC_LEFT_GUI),
};

static void bracket_pairs_r10_shortcut_clears_stack(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(shortcut_modifiers); index++) {
        start_case();
        hold_key(keycode_of(opening_parenthesis_key));
        set_fake_modifiers(shortcut_modifiers[index]);
        assert_true(push_and_release(KC_S, NOT_A_TAP));
        set_fake_modifiers(0);
        assert_true(push_and_release(KC_TAB, NOT_A_TAP));
    }
}

static void bracket_pairs_r10_shift_tab_clears_stack(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Shift with Tab moves the text, so the cursor can move
    set_fake_modifiers(LEFT_SHIFT_BIT);
    assert_true(push_and_release(KC_TAB, NOT_A_TAP));
    set_fake_modifiers(0);
    assert_true(push_and_release(KC_TAB, NOT_A_TAP));
}

static void bracket_pairs_r11_ac8_other_closer_clears_stack(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // "]" hold on the period key
    hold_key(keycode_of(closing_square_bracket_key));
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_REGISTER_CODE16, KC_RIGHT_BRACKET},
        {CALLED_UNREGISTER_CODE16, KC_RIGHT_BRACKET},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r12_ac9_wait_then_tab(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    advance_fake_clock(LONGER_THAN_IDLE_TIME);
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r13_ac11_empty_stack_tab_and_closer(void **state) {
    (void)state;
    start_case();
    type_key(KC_TAB);
    hold_key(keycode_of(closing_parenthesis_key));
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_TAB},
        {CALLED_REGISTER_CODE16, KC_RIGHT_PAREN},
        {CALLED_UNREGISTER_CODE16, KC_RIGHT_PAREN},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/** Layer state while the special mode is on */
#define SPECIAL_MODE_ON ((layer_state_t)(1U << LAYER_SPECIAL))

/** Calls of a double quote with the right home row Shift held */
#define DOUBLE_QUOTE_PAIR_CALLS                                               \
    {CALLED_DEL_MODS, RIGHT_SHIFT_BIT}, {CALLED_TAP_CODE16, KC_DOUBLE_QUOTE}, \
        {CALLED_TAP_CODE16, KC_DOUBLE_QUOTE}, {CALLED_TAP_CODE16, KC_LEFT}, { \
        CALLED_ADD_MODS, RIGHT_SHIFT_BIT                                      \
    }

/** Types a double quote: the quote key (2,13) while A (2,9), the right Shift, is held
 */
static void type_double_quote(void) {
    assert_true(push_and_release(keycode_of(right_shift_key), NOT_A_TAP));
    set_fake_modifiers(RIGHT_SHIFT_BIT);
    type_key(keycode_of(quote_key));
    set_fake_modifiers(0);
}

static void bracket_pairs_r16_r18_ac12_double_quote_sends_pair(void **state) {
    (void)state;
    start_case();
    type_double_quote();
    static const qmk_call_t expected[] = {DOUBLE_QUOTE_PAIR_CALLS};
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r16_ac15_double_quote_in_parentheses(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_double_quote();
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        DOUBLE_QUOTE_PAIR_CALLS,
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r17_ac13_second_double_quote_sends_right(void **state) {
    (void)state;
    start_case();
    type_double_quote();
    type_key(KC_A);
    type_key(KC_B);
    type_double_quote();
    static const qmk_call_t expected[] = {
        DOUBLE_QUOTE_PAIR_CALLS,       {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_B},     {CALLED_DEL_MODS, RIGHT_SHIFT_BIT},
        {CALLED_TAP_CODE16, KC_RIGHT}, {CALLED_ADD_MODS, RIGHT_SHIFT_BIT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r16_r17_ac14_backtick_pair_in_special_mode(void **state) {
    (void)state;
    start_case();
    layer_state       = SPECIAL_MODE_ON;
    uint16_t backtick = keycode_of(backtick_key);
    type_key(backtick);
    // Digit 1 of the special mode at (3,9)
    type_key(keycode_of(special_digit_one_key));
    type_key(backtick);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_GRAVE}, {CALLED_TAP_CODE16, KC_GRAVE},
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_1},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r16_backtick_only_in_special_mode(void **state) {
    (void)state;
    start_case();
    type_key(KC_GRAVE);
    // With Shift, the backtick key types a tilde
    layer_state = SPECIAL_MODE_ON;
    set_fake_modifiers(LEFT_SHIFT_BIT);
    type_key(KC_GRAVE);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_GRAVE},
        {CALLED_TAP_CODE16, KC_GRAVE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_single_quote_is_one_character(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(keycode_of(quote_key));
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_QUOTE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r15_paused_makes_no_pairs(void **state) {
    (void)state;
    start_case();
    bracket_pairs_set_paused(true);
    hold_key(keycode_of(opening_parenthesis_key));
    type_double_quote();
    type_key(KC_TAB);
    bracket_pairs_set_paused(false);
    hold_key(keycode_of(opening_parenthesis_key));
    static const qmk_call_t expected[] = {
        {CALLED_REGISTER_CODE16, KC_LEFT_PAREN},
        {CALLED_UNREGISTER_CODE16, KC_LEFT_PAREN},
        {CALLED_TAP_CODE16, KC_QUOTE},
        {CALLED_TAP_CODE16, KC_TAB},
        PARENTHESES_PAIR_CALLS,
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r15_pause_clears_stack(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    bracket_pairs_set_paused(true);
    bracket_pairs_set_paused(false);
    assert_true(push_and_release(KC_TAB, NOT_A_TAP));
}

/** Starts the special mode with its mode key, then empties the call log */
static void start_special_mode(void) {
    start_case();
    send_key_event(SPECIAL_MODE_KEY, true);
    send_key_event(SPECIAL_MODE_KEY, false);
    reset_fake_qmk();
    layer_state = SPECIAL_MODE_ON;
}

static void bracket_pairs_taken_push_keeps_special_mode_on(void **state) {
    (void)state;
    start_special_mode();
    uint16_t backtick = keycode_of(backtick_key);
    // Space, a backtick pair, Backspace that deletes the pair, then Space
    type_key(KC_SPACE);
    type_key(backtick);
    type_key(KC_BACKSPACE);
    type_key(KC_SPACE);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_SPACE},     {CALLED_TAP_CODE16, KC_GRAVE},
        {CALLED_TAP_CODE16, KC_GRAVE},     {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_BACKSPACE}, {CALLED_TAP_CODE16, KC_DELETE},
        {CALLED_TAP_CODE16, KC_SPACE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_jump_out_tab_keeps_special_mode_on(void **state) {
    (void)state;
    start_special_mode();
    uint16_t backtick = keycode_of(backtick_key);
    type_key(KC_TAB);
    type_key(backtick);
    // Jump-out Tab, then a Tab after the pair
    type_key(KC_TAB);
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_TAB},   {CALLED_TAP_CODE16, KC_GRAVE},
        {CALLED_TAP_CODE16, KC_GRAVE}, {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_RIGHT}, {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_space_that_ends_special_mode_types_nothing(void **state) {
    (void)state;
    start_special_mode();
    type_key(keycode_of(backtick_key));
    type_key(KC_A);
    // Second Space ends the special mode and types nothing
    type_key(KC_SPACE);
    type_key(KC_SPACE);
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    type_key(KC_BACKSPACE);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_GRAVE},     {CALLED_TAP_CODE16, KC_GRAVE},
        {CALLED_TAP_CODE16, KC_LEFT},      {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_SPACE},     {CALLED_LAYER_OFF, LAYER_SPECIAL},
        {CALLED_TAP_CODE16, KC_BACKSPACE}, {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE}, {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_clear_removes_pairs(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    bracket_pairs_clear();
    assert_true(push_and_release(KC_TAB, NOT_A_TAP));
}

/** Cases of this program, in the order of the spec requirements */
static const struct CMUnitTest bracket_pairs_cases[] = {
    cmocka_unit_test(bracket_pairs_r1_ac1_ac10_opener_hold_sends_pair),
    cmocka_unit_test(bracket_pairs_release_of_taken_push_stops),
    cmocka_unit_test(bracket_pairs_r3_ac6_shift_opener_sends_only_opener),
    cmocka_unit_test(bracket_pairs_r3_shift_square_bracket_sends_a_square_bracket),
    cmocka_unit_test(bracket_pairs_r4_each_character_counts),
    cmocka_unit_test(bracket_pairs_r4_case_mode_underscore_counts),
    cmocka_unit_test(bracket_pairs_r4_case_mode_capital_counts),
    cmocka_unit_test(bracket_pairs_r4_sent_backspace_counts),
    cmocka_unit_test(bracket_pairs_r5_ac2_tab_sends_right),
    cmocka_unit_test(bracket_pairs_r6_ac3_matching_closer_sends_right),
    cmocka_unit_test(bracket_pairs_r7_ac4_second_tab_leaves_outer_pair),
    cmocka_unit_test(bracket_pairs_r8_r9_ac5_backspace_empties_line),
    cmocka_unit_test(bracket_pairs_r9_outputs_with_no_shift),
    cmocka_unit_test(bracket_pairs_r10_clearing_key_clears_stack),
    cmocka_unit_test(bracket_pairs_r10_ac7_mouse_key_then_tab),
    cmocka_unit_test(bracket_pairs_r10_shortcut_clears_stack),
    cmocka_unit_test(bracket_pairs_r10_shift_tab_clears_stack),
    cmocka_unit_test(bracket_pairs_r11_ac8_other_closer_clears_stack),
    cmocka_unit_test(bracket_pairs_r12_ac9_wait_then_tab),
    cmocka_unit_test(bracket_pairs_r13_ac11_empty_stack_tab_and_closer),
    cmocka_unit_test(bracket_pairs_r15_paused_makes_no_pairs),
    cmocka_unit_test(bracket_pairs_r15_pause_clears_stack),
    cmocka_unit_test(bracket_pairs_r16_r18_ac12_double_quote_sends_pair),
    cmocka_unit_test(bracket_pairs_r16_ac15_double_quote_in_parentheses),
    cmocka_unit_test(bracket_pairs_r17_ac13_second_double_quote_sends_right),
    cmocka_unit_test(bracket_pairs_r16_r17_ac14_backtick_pair_in_special_mode),
    cmocka_unit_test(bracket_pairs_r16_backtick_only_in_special_mode),
    cmocka_unit_test(bracket_pairs_single_quote_is_one_character),
    cmocka_unit_test(bracket_pairs_taken_push_keeps_special_mode_on),
    cmocka_unit_test(bracket_pairs_jump_out_tab_keeps_special_mode_on),
    cmocka_unit_test(bracket_pairs_r4_space_that_ends_special_mode_types_nothing),
    cmocka_unit_test(bracket_pairs_clear_removes_pairs),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(bracket_pairs_cases, NULL, NULL);
}
