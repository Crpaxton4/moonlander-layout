/** @file
 * Tests of the bracket and quote pairs through the keymap's key event hook, with
 * the recording fake of QMK.
 *
 * Each key comes from its spec position in the keymap. When the hook lets a key
 * pass, QMK sends that key. Each case models this with a tap of the key in the call
 * log, so the log is all that the host gets, in order. The symbol holds register
 * and unregister their symbols themselves.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "keymap_introspection.h"

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

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

/** Number of grid columns of the two halves */
#define GRID_COLUMN_COUNT 14

/** Matrix rows from a left half row to the same row of the right half */
#define RIGHT_HALF_ROW_OFFSET RIGHT_NUMBER_ROW

/** Left Shift bit, as the hold of the N home row key sets it */
#define LEFT_SHIFT_BIT MOD_BIT(KC_LEFT_SHIFT)

/** Right Shift bit, as the hold of the A home row key sets it */
#define RIGHT_SHIFT_BIT MOD_BIT(KC_RIGHT_SHIFT)

/** Time in ms of the wait of AC9, longer than the idle time */
#define LONGER_THAN_IDLE_TIME 31000

/** Backspace pushes that empty the pair after snake_case "a b" */
#define UNDERSCORE_CASE_BACKSPACES 4

/** Backspace pushes that empty the pair after camelCase "a b" */
#define CAPITAL_CASE_BACKSPACES 3

/** Grid row of the bracket holds */
#define BRACKET_ROW 3

/** Grid row of the Esc and F-key holds */
#define TOP_LETTER_ROW 1

/** Number of Esc and F-key holds, from (1,0) to (1,12) */
#define TOP_ROW_HOLD_COUNT 13

/** Calls of a hold of V, the "(" hold */
#define PARENTHESES_PAIR_CALLS                                                 \
    {CALLED_TAP_CODE16, KC_LEFT_PAREN}, {CALLED_TAP_CODE16, KC_RIGHT_PAREN}, { \
        CALLED_TAP_CODE16, KC_LEFT                                             \
    }

/** Layer state while the special mode is on */
#define SPECIAL_MODE_ON ((layer_state_t)(1U << LAYER_SPECIAL))

/** Layer state while the nav mode is on */
#define NAV_MODE_ON ((layer_state_t)(1U << LAYER_NAV))

/** Calls of the letters "abc" */
#define ABC_CALLS                                           \
    {CALLED_TAP_CODE16, KC_A}, {CALLED_TAP_CODE16, KC_B}, { \
        CALLED_TAP_CODE16, KC_C                             \
    }

/** Hold time in ms of a key that the host repeats */
#define REPEAT_SAFE_TIME 400

/** Hold time in ms of a key that the host does not repeat */
#define JUST_BEFORE_REPEAT_SAFE_TIME (REPEAT_SAFE_TIME - 1)

/** Hold time in ms of the long holds of the flash test */
#define ONE_SECOND 1000

/** Calls of a double quote with the right home row Shift held */
#define DOUBLE_QUOTE_PAIR_CALLS                                               \
    {CALLED_DEL_MODS, RIGHT_SHIFT_BIT}, {CALLED_TAP_CODE16, KC_DOUBLE_QUOTE}, \
        {CALLED_TAP_CODE16, KC_DOUBLE_QUOTE}, {CALLED_TAP_CODE16, KC_LEFT}, { \
        CALLED_ADD_MODS, RIGHT_SHIFT_BIT                                      \
    }

// #endregion

// #region Types

/** Key position of the spec */
typedef struct {
    uint8_t layer;  /**< Layer of the key */
    uint8_t row;    /**< Grid row */
    uint8_t column; /**< Grid column */
} grid_position_t;

/** Push of a key that the operator holds for a time */
typedef struct {
    uint16_t keycode;   /**< Keycode of a key that is not a tap-hold key */
    uint32_t hold_time; /**< Time in ms between the push and the release */
} held_push_t;

/** Bracket hold of the spec and the symbols of its pair */
typedef struct {
    uint8_t  row;    /**< Grid row of the opener hold */
    uint8_t  column; /**< Grid column of the opener hold */
    uint16_t opener; /**< Symbol of the opener */
    uint16_t closer; /**< Symbol of the matching closer */
} bracket_pair_t;

// #endregion

// #region Data

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

/** Mouse right key N (2,4) of the nav mode */
static const grid_position_t mouse_right_key = {
    .layer  = LAYER_NAV,
    .row    = 2,
    .column = 4,
};

/** B (1,11), a plain letter key */
static const grid_position_t letter_b_key = {
    .layer  = LAYER_BASE,
    .row    = 1,
    .column = 11,
};

/** K (3,8), the "-" hold */
static const grid_position_t minus_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 8,
};

/** Slash (3,12), the ">" hold */
static const grid_position_t closing_angle_bracket_key = {
    .layer  = LAYER_BASE,
    .row    = 3,
    .column = 12,
};

/** Left A (2,9) of the nav mode */
static const grid_position_t left_arrow_key = {
    .layer  = LAYER_NAV,
    .row    = 2,
    .column = 9,
};

/** Up U (1,10) of the nav mode */
static const grid_position_t up_arrow_key = {
    .layer  = LAYER_NAV,
    .row    = 1,
    .column = 10,
};

/** Right O (2,11) of the nav mode */
static const grid_position_t right_arrow_key = {
    .layer  = LAYER_NAV,
    .row    = 2,
    .column = 11,
};

/** Tab (1,0) of the nav mode, with no Esc hold */
static const grid_position_t nav_tab_key = {
    .layer  = LAYER_NAV,
    .row    = 1,
    .column = 0,
};

/**
 * Precision key R1 of the nav mode. The thumb keys have no grid row. key_at() moves
 * the left thumb row to the right thumb row for a column of the right half.
 */
static const grid_position_t precision_key = {
    .layer  = LAYER_NAV,
    .row    = LEFT_THUMB_KEYS,
    .column = FIRST_RIGHT_COLUMN + RIGHT_OUTER_THUMB,
};

/** Backtick (1,1) of the special mode */
static const grid_position_t backtick_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 1,
    .column = 1,
};

/** Opening parenthesis (1,4) of the special mode, a plain character */
static const grid_position_t special_opening_parenthesis_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 1,
    .column = 4,
};

/** Closing parenthesis (4,2) of the special mode, a plain character */
static const grid_position_t special_closing_parenthesis_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 4,
    .column = 2,
};

/** Digit 1 (3,9) of the special mode */
static const grid_position_t special_digit_one_key = {
    .layer  = LAYER_SPECIAL,
    .row    = 3,
    .column = 9,
};

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

/**
 * Keys of the nav mode that move the text cursor to a place that the pairs do not
 * know: mouse keys, mouse buttons, wheel keys, Up, Down, Home and End.
 */
static const grid_position_t nav_clearing_keys[] = {
    {.layer = LAYER_NAV, .row = 1, .column = 9},
    {.layer = LAYER_NAV, .row = 1, .column = 11},
    {.layer = LAYER_NAV, .row = 3, .column = 9},
    {.layer = LAYER_NAV, .row = 3, .column = 10},
    {.layer = LAYER_NAV, .row = 3, .column = 11},
    {.layer = LAYER_NAV, .row = 1, .column = 3},
    {.layer = LAYER_NAV, .row = 2, .column = 2},
    {.layer = LAYER_NAV, .row = 2, .column = 3},
    {.layer = LAYER_NAV, .row = 2, .column = 4},
    {.layer = LAYER_NAV, .row = 1, .column = 8},
    {.layer = LAYER_NAV, .row = 2, .column = 8},
    {.layer = LAYER_NAV, .row = 1, .column = 10},
    {.layer = LAYER_NAV, .row = 2, .column = 10},
    {.layer = LAYER_NAV, .row = 1, .column = 12},
    {.layer = LAYER_NAV, .row = 2, .column = 12},
};

/** Closer holds of the spec, in the order of bracket_pairs */
static const uint8_t closer_columns[] = {9, 11, 10, 12};

/** Keys of spec 33 R10 that clear the pair stack, as the keymap has them */
static const uint16_t clearing_keys[] = {
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
};

/** Modifier bits of the Ctrl, Alt and GUI home row holds */
static const uint8_t shortcut_modifiers[] = {
    MOD_BIT(KC_LEFT_CTRL),
    MOD_BIT(KC_LEFT_ALT),
    MOD_BIT(KC_LEFT_GUI),
};

/** Modifier bits of each home row hold */
static const uint8_t home_row_modifiers[] = {
    MOD_BIT(KC_LEFT_SHIFT), MOD_BIT(KC_RIGHT_SHIFT), MOD_BIT(KC_LEFT_CTRL),
    MOD_BIT(KC_LEFT_ALT),   MOD_BIT(KC_LEFT_GUI),
};

/** Left and Right, which keep the pairs only with no modifier */
static const uint16_t left_and_right_keys[] = {
    KC_LEFT,
    KC_RIGHT,
};

// #endregion

// #region Static functions

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
 * Gives the keycode of thumb key L2, Backspace, of the base layer.
 *
 * @return Keycode of L2.
 */
static uint16_t backspace_key(void) {
    return keycode_at_keymap_location(LAYER_BASE, LEFT_THUMB_KEYS, LEFT_MIDDLE_THUMB);
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
 * Taps a case mode key. Case modes take the push and let the release pass.
 *
 * @param keycode Keycode of the mode key.
 */
static void push_mode_key(uint16_t keycode) {
    // Snake and camel keys are tap-hold keys; the other mode keys ignore the count
    keyrecord_t push    = {.event = {.pressed = true}, .tap = {.count = ONE_TAP}};
    keyrecord_t release = {.event = {.pressed = false}, .tap = {.count = ONE_TAP}};
    assert_false(process_record_user(keycode, &push));
    assert_true(process_record_user(keycode, &release));
}

/** Clears the fake, the pair stack and any case mode of an earlier case */
static void start_case(void) {
    reset_fake_qmk();
    bracket_pairs_clear();
    case_modes_end_all();
}

/**
 * Pushes thumb key R0 of the default mode. The hook lets it pass, and QMK then
 * turns on the nav layer, so the test turns it on.
 */
static void turn_on_nav_mode(void) {
    uint16_t nav_mode_key =
        keycode_at_keymap_location(LAYER_BASE, RIGHT_THUMB_KEYS, RIGHT_LARGE_THUMB);
    assert_true(push_and_release(nav_mode_key, NOT_A_TAP));
    layer_state = NAV_MODE_ON;
}

/**
 * Pushes thumb key R0 of the nav mode, which is the key of the default mode below
 * it. The hook lets it pass, and QMK then turns off the nav layer, so the test turns
 * it off.
 */
static void turn_off_nav_mode(void) {
    uint16_t nav_mode_key =
        keycode_at_keymap_location(LAYER_BASE, RIGHT_THUMB_KEYS, RIGHT_LARGE_THUMB);
    assert_int_equal(
        keycode_at_keymap_location(LAYER_NAV, RIGHT_THUMB_KEYS, RIGHT_LARGE_THUMB),
        KC_TRANSPARENT);
    assert_true(push_and_release(nav_mode_key, NOT_A_TAP));
    layer_state = 0;
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
    tap_tap_hold_key(keycode_of(tab_key));
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

static void bracket_pairs_r10_clearing_key_clears_stack(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(clearing_keys); index++) {
        start_case();
        hold_key(keycode_of(opening_parenthesis_key));
        push_and_release(clearing_keys[index], NOT_A_TAP);
        assert_true(push_and_release(KC_TAB, NOT_A_TAP));
    }
}

static void bracket_pairs_r10_top_row_hold_clears_stack(void **state) {
    (void)state;
    for (uint8_t column = 0; column < TOP_ROW_HOLD_COUNT; column++) {
        start_case();
        hold_key(keycode_of(opening_parenthesis_key));
        // Holds of Tab and the top letter row send Esc and the F-keys
        hold_key(base_key_at(TOP_LETTER_ROW, column));
        assert_true(push_and_release(keycode_of(tab_key), A_TAP));
    }
}

static void bracket_pairs_r10_ac7_mouse_key_then_tab(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_X);
    turn_on_nav_mode();
    // Mouse right key; QMK moves the cursor and sends no key
    assert_true(push_and_release(keycode_of(mouse_right_key), NOT_A_TAP));
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_X},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

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

static void bracket_pairs_r11_ac8_ht14_other_closer_is_a_character(void **state) {
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
        {CALLED_TAP_CODE16, KC_RIGHT},
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

static void bracket_pairs_special_mode_parentheses_are_plain_characters(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    send_key_event(SPECIAL_MODE_KEY, true);
    send_key_event(SPECIAL_MODE_KEY, false);
    reset_fake_qmk();
    layer_state = SPECIAL_MODE_ON;
    // No pair for "(" and no jump-out for ")", so Tab jumps out of the pair of the hold
    type_key(keycode_of(special_opening_parenthesis_key));
    type_key(keycode_of(special_closing_parenthesis_key));
    type_key(KC_TAB);
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_LEFT_PAREN},
        {CALLED_TAP_CODE16, KC_RIGHT_PAREN},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
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

/** Holds V (3,4) for "(" and types "abc", so the host has "(abc|)" */
static void open_pair_and_type_abc(void) {
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_B);
    type_key(KC_C);
}

/**
 * Pushes a key that is not a tap-hold key, holds it and releases it. A key that
 * passes goes to the call log as a tap, as QMK sends it.
 *
 * @param push Key and hold time.
 */
static void type_held_key(held_push_t push) {
    if (send_key_event(push.keycode, true)) {
        tap_code16(push.keycode);
    }
    advance_fake_clock(push.hold_time);
    send_key_event(push.keycode, false);
}

static void bracket_pairs_flash_closer_hold_after_text_jumps_out(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    hold_key(keycode_of(closing_parenthesis_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_flash_tab_after_text_jumps_out(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_flash_quick_tap_of_closer_key_types_letter(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    // Quick tap of P is the letter, which the pair counts
    tap_tap_hold_key(keycode_of(closing_parenthesis_key));
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_P},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_flash_nav_mode_key_keeps_pair(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    turn_off_nav_mode();
    hold_key(keycode_of(closing_parenthesis_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r19_r6_ht1_ac16_flash_left_then_closer(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(left_arrow_key));
    turn_off_nav_mode();
    hold_key(keycode_of(closing_parenthesis_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_RIGHT}, {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r19_r5_ht2_ac17_flash_left_then_tab(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_RIGHT}, {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r21_ht3_ac21_flash_left_right_then_tab(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(right_arrow_key));
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r11_r6_ht13_ac20_flash_operator_in_pair(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    tap_tap_hold_key(keycode_of(closing_parenthesis_key));
    hold_key(keycode_of(minus_key));
    hold_key(keycode_of(closing_angle_bracket_key));
    type_key(KC_X);
    hold_key(keycode_of(closing_parenthesis_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        {CALLED_TAP_CODE16, KC_P},
        {CALLED_REGISTER_CODE16, KC_MINUS},
        {CALLED_UNREGISTER_CODE16, KC_MINUS},
        {CALLED_REGISTER_CODE16, KC_RIGHT_ANGLE_BRACKET},
        {CALLED_UNREGISTER_CODE16, KC_RIGHT_ANGLE_BRACKET},
        {CALLED_TAP_CODE16, KC_X},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_ac24_flash_held_backspace_forgets_pairs(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    type_held_key((held_push_t){.keycode = KC_BACKSPACE, .hold_time = ONE_SECOND});
    assert_false(bracket_pairs_are_open());
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_ac24_flash_keymap_keys_then_held_backspace(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Keymap keys of "abc": the A mod-tap, the B F-key hold and the C symbol hold
    tap_tap_hold_key(keycode_of(right_shift_key));
    tap_tap_hold_key(keycode_of(letter_b_key));
    tap_tap_hold_key(keycode_of(opening_curly_brace_key));
    type_held_key((held_push_t){
        .keycode   = backspace_key(),
        .hold_time = ONE_SECOND,
    });
    assert_false(bracket_pairs_are_open());
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_backspace_released_before_safe_time_keeps_pair(
    void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    // Host repeats Backspace after its own delay; a shorter hold keeps the pair
    type_held_key((held_push_t){
        .keycode   = backspace_key(),
        .hold_time = JUST_BEFORE_REPEAT_SAFE_TIME,
    });
    assert_true(bracket_pairs_are_open());
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r19_r4_ht4_ac18_letter_after_left_then_tab(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_C);
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    // B (1,11) is the right click in the nav mode, so the letter needs the default mode
    turn_off_nav_mode();
    type_key(KC_B);
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_C},     {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_B},     {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r20_r13_ht5_left_past_opener_forgets_pairs(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,       {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_LEFT}, {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r22_r7_ht6_right_at_closer_leaves_inner_pair(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_curly_brace_key));
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    turn_on_nav_mode();
    type_key(keycode_of(right_arrow_key));
    type_key(keycode_of(nav_tab_key));
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

static void bracket_pairs_r23_ht7_backspace_at_opener_forgets_pairs(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_B);
    turn_on_nav_mode();
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(left_arrow_key));
    type_key(KC_BACKSPACE);
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,       {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_B},    {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_LEFT}, {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r10_ht8_left_and_right_with_modifier_clear(void **state) {
    (void)state;
    for (size_t key = 0; key < ARRAY_SIZE(left_and_right_keys); key++) {
        for (size_t mod = 0; mod < ARRAY_SIZE(home_row_modifiers); mod++) {
            start_case();
            open_pair_and_type_abc();
            set_fake_modifiers(home_row_modifiers[mod]);
            assert_true(push_and_release(left_and_right_keys[key], NOT_A_TAP));
            set_fake_modifiers(0);
            assert_true(push_and_release(KC_TAB, NOT_A_TAP));
        }
    }
}

static void bracket_pairs_r10_ht8_shift_left_then_tab(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    // Shift with Left selects text, which the next key can replace
    set_fake_modifiers(LEFT_SHIFT_BIT);
    type_key(keycode_of(left_arrow_key));
    set_fake_modifiers(0);
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r10_ht9_up_forgets_pairs(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_key(keycode_of(up_arrow_key));
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_UP},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_ht10_left_held_for_safe_time_forgets_pairs(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_held_key((held_push_t){
        .keycode   = keycode_of(left_arrow_key),
        .hold_time = REPEAT_SAFE_TIME,
    });
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_ht11_left_released_before_safe_time(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    type_held_key((held_push_t){
        .keycode   = keycode_of(left_arrow_key),
        .hold_time = JUST_BEFORE_REPEAT_SAFE_TIME,
    });
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_ht12_backspace_held_for_safe_time(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    type_key(KC_A);
    type_key(KC_B);
    type_held_key((held_push_t){
        .keycode   = KC_BACKSPACE,
        .hold_time = REPEAT_SAFE_TIME,
    });
    tap_tap_hold_key(keycode_of(tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,      {CALLED_TAP_CODE16, KC_A},
        {CALLED_TAP_CODE16, KC_B},   {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_TAB},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r24_long_symbol_hold_forgets_pairs(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Host repeats the "-" of a long hold of K
    assert_false(send_key_event(keycode_of(minus_key), true));
    advance_fake_clock(REPEAT_SAFE_TIME);
    assert_false(send_key_event(keycode_of(minus_key), false));
    assert_true(push_and_release(KC_TAB, NOT_A_TAP));
}

static void bracket_pairs_r24_long_modifier_hold_keeps_pairs(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    // Host does not repeat a modifier, so a long Shift hold keeps the pair
    assert_true(send_key_event(keycode_of(left_shift_key), true));
    advance_fake_clock(ONE_SECOND);
    assert_true(send_key_event(keycode_of(left_shift_key), false));
    assert_false(push_and_release(KC_TAB, NOT_A_TAP));
}

static void bracket_pairs_r24_long_hold_of_taken_key_keeps_pairs(void **state) {
    (void)state;
    start_case();
    // Module sent the pair with taps, so the host repeats nothing
    assert_false(send_key_event(keycode_of(opening_parenthesis_key), true));
    advance_fake_clock(ONE_SECOND);
    assert_false(send_key_event(keycode_of(opening_parenthesis_key), false));
    assert_true(bracket_pairs_are_open());
}

static void bracket_pairs_r25_pairs_are_open_until_jump_out(void **state) {
    (void)state;
    start_case();
    assert_false(bracket_pairs_are_open());
    hold_key(keycode_of(opening_parenthesis_key));
    assert_true(bracket_pairs_are_open());
    type_key(KC_TAB);
    assert_false(bracket_pairs_are_open());
}

static void bracket_pairs_r25_r12_pairs_are_not_open_after_idle_time(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    advance_fake_clock(LONGER_THAN_IDLE_TIME);
    assert_false(bracket_pairs_are_open());
}

static void bracket_pairs_r10_nav_mode_keys_clear_stack(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(nav_clearing_keys); index++) {
        start_case();
        hold_key(keycode_of(opening_parenthesis_key));
        turn_on_nav_mode();
        uint16_t keycode = keycode_of(nav_clearing_keys[index]);
        assert_true(push_and_release(keycode, NOT_A_TAP));
        assert_false(bracket_pairs_are_open());
    }
}

static void bracket_pairs_nav_mode_precision_key_keeps_pair(void **state) {
    (void)state;
    start_case();
    open_pair_and_type_abc();
    turn_on_nav_mode();
    // Precision key moves no cursor, so the pair stays
    assert_false(push_and_release(keycode_of(precision_key), NOT_A_TAP));
    assert_true(bracket_pairs_are_open());
    type_key(keycode_of(nav_tab_key));
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,
        ABC_CALLS,
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_r4_nav_mode_space_tap_counts(void **state) {
    (void)state;
    start_case();
    hold_key(keycode_of(opening_parenthesis_key));
    turn_on_nav_mode();
    // Thumb key L1 of the nav mode is the Space of the default mode
    assert_int_equal(
        keycode_at_keymap_location(LAYER_NAV, LEFT_THUMB_KEYS, LEFT_OUTER_THUMB),
        KC_TRANSPARENT);
    type_key(KC_SPACE);
    type_key(keycode_of(left_arrow_key));
    type_key(keycode_of(nav_tab_key));
    // Space and Left are in the pair, so the jump-out sends Right two times
    static const qmk_call_t expected[] = {
        PARENTHESES_PAIR_CALLS,        {CALLED_TAP_CODE16, KC_SPACE},
        {CALLED_TAP_CODE16, KC_LEFT},  {CALLED_TAP_CODE16, KC_RIGHT},
        {CALLED_TAP_CODE16, KC_RIGHT},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/**
 * Fails the test if a long push in the nav mode of a bottom letter row key opens a
 * pair, or does not pass to QMK. A column with no hold in the default mode has no
 * check.
 *
 * @param column Grid column of the key.
 */
static void assert_nav_key_opens_no_pair(uint8_t column) {
    if (!IS_QK_LAYER_TAP(base_key_at(BRACKET_ROW, column))) {
        return;
    }
    uint16_t nav_key = key_at(LAYER_NAV, BRACKET_ROW, column);
    start_case();
    turn_on_nav_mode();
    type_key(nav_key);
    assert_false(bracket_pairs_are_open());
    const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, nav_key},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void bracket_pairs_nav_mode_bottom_letter_row_has_no_opener(void **state) {
    (void)state;
    for (uint8_t column = 0; column < GRID_COLUMN_COUNT; column++) {
        assert_nav_key_opens_no_pair(column);
    }
}

// #endregion

// #region Test runner

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
    cmocka_unit_test(bracket_pairs_r10_top_row_hold_clears_stack),
    cmocka_unit_test(bracket_pairs_r10_ac7_mouse_key_then_tab),
    cmocka_unit_test(bracket_pairs_r10_nav_mode_keys_clear_stack),
    cmocka_unit_test(bracket_pairs_nav_mode_precision_key_keeps_pair),
    cmocka_unit_test(bracket_pairs_r4_nav_mode_space_tap_counts),
    cmocka_unit_test(bracket_pairs_nav_mode_bottom_letter_row_has_no_opener),
    cmocka_unit_test(bracket_pairs_r10_shortcut_clears_stack),
    cmocka_unit_test(bracket_pairs_r10_shift_tab_clears_stack),
    cmocka_unit_test(bracket_pairs_r11_ac8_ht14_other_closer_is_a_character),
    cmocka_unit_test(bracket_pairs_r12_ac9_wait_then_tab),
    cmocka_unit_test(bracket_pairs_r13_ac11_empty_stack_tab_and_closer),
    cmocka_unit_test(bracket_pairs_r16_r18_ac12_double_quote_sends_pair),
    cmocka_unit_test(bracket_pairs_r16_ac15_double_quote_in_parentheses),
    cmocka_unit_test(bracket_pairs_r17_ac13_second_double_quote_sends_right),
    cmocka_unit_test(bracket_pairs_r16_r17_ac14_backtick_pair_in_special_mode),
    cmocka_unit_test(bracket_pairs_r16_backtick_only_in_special_mode),
    cmocka_unit_test(bracket_pairs_single_quote_is_one_character),
    cmocka_unit_test(bracket_pairs_special_mode_parentheses_are_plain_characters),
    cmocka_unit_test(bracket_pairs_taken_push_keeps_special_mode_on),
    cmocka_unit_test(bracket_pairs_jump_out_tab_keeps_special_mode_on),
    cmocka_unit_test(bracket_pairs_r4_space_that_ends_special_mode_types_nothing),
    cmocka_unit_test(bracket_pairs_clear_removes_pairs),
    cmocka_unit_test(bracket_pairs_flash_closer_hold_after_text_jumps_out),
    cmocka_unit_test(bracket_pairs_flash_tab_after_text_jumps_out),
    cmocka_unit_test(bracket_pairs_flash_quick_tap_of_closer_key_types_letter),
    cmocka_unit_test(bracket_pairs_flash_nav_mode_key_keeps_pair),
    cmocka_unit_test(bracket_pairs_r19_r6_ht1_ac16_flash_left_then_closer),
    cmocka_unit_test(bracket_pairs_r19_r5_ht2_ac17_flash_left_then_tab),
    cmocka_unit_test(bracket_pairs_r21_ht3_ac21_flash_left_right_then_tab),
    cmocka_unit_test(bracket_pairs_r11_r6_ht13_ac20_flash_operator_in_pair),
    cmocka_unit_test(bracket_pairs_r24_ac24_flash_held_backspace_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r24_ac24_flash_keymap_keys_then_held_backspace),
    cmocka_unit_test(bracket_pairs_r24_backspace_released_before_safe_time_keeps_pair),
    cmocka_unit_test(bracket_pairs_r19_r4_ht4_ac18_letter_after_left_then_tab),
    cmocka_unit_test(bracket_pairs_r20_r13_ht5_left_past_opener_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r22_r7_ht6_right_at_closer_leaves_inner_pair),
    cmocka_unit_test(bracket_pairs_r23_ht7_backspace_at_opener_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r10_ht8_left_and_right_with_modifier_clear),
    cmocka_unit_test(bracket_pairs_r10_ht8_shift_left_then_tab),
    cmocka_unit_test(bracket_pairs_r10_ht9_up_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r24_ht10_left_held_for_safe_time_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r24_ht11_left_released_before_safe_time),
    cmocka_unit_test(bracket_pairs_r24_ht12_backspace_held_for_safe_time),
    cmocka_unit_test(bracket_pairs_r24_long_symbol_hold_forgets_pairs),
    cmocka_unit_test(bracket_pairs_r24_long_modifier_hold_keeps_pairs),
    cmocka_unit_test(bracket_pairs_r24_long_hold_of_taken_key_keeps_pairs),
    cmocka_unit_test(bracket_pairs_r25_pairs_are_open_until_jump_out),
    cmocka_unit_test(bracket_pairs_r25_r12_pairs_are_not_open_after_idle_time),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(bracket_pairs_cases, NULL, NULL);
}

// #endregion
