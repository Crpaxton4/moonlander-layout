/** @file
 * Tests of the case modes through the keymap's key event hook, with the recording
 * fake of QMK.
 *
 * When the hook lets a key pass, QMK sends that key. Each case models this with a
 * tap of the key in the call log, so the log is all that the host gets, in order.
 * Each release must pass, because the hook acts only on pushes.
 */

// #region Includes

#include <ctype.h>

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "keycodes/keycodes.h"
#include "recorded_calls.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Tap count of a key that is not a tap-hold key, or of a tap-hold hold */
#define NO_TAP 0

/** Tap count of the tap of a tap-hold key */
#define ONE_TAP 1

/** Value of push_key() is_tap for a key that is not a tap-hold key, or for a hold */
#define NOT_A_TAP false

/** Value of push_key() is_tap for the tap of a tap-hold key */
#define A_TAP true

/** Most keys that one case gives to assert_host_got() */
#define MOST_HOST_KEYS 64

// #endregion

// #region Types

/** Character of a test text and the key that types it, for keys that are not letters */
typedef struct {
    char     character; /**< Character in a test text */
    uint16_t keycode;   /**< Key that types it on a US host layout */
} character_key_t;

/** Home row mod and the modifier that its hold gives */
typedef struct {
    uint16_t keycode;  /**< Mod-tap keycode of the home row key */
    uint8_t  modifier; /**< Modifier bit that QMK sets while the hold lasts */
} home_row_mod_t;

// #endregion

// #region Data

/** Keys of the characters other than letters and digits */
static const character_key_t character_keys[] = {
    {' ', KC_SPACE},
    {'\b', KC_BACKSPACE},
    {'_', KC_UNDERSCORE},
};

/** Home row mods of the right hand that make a shortcut */
static const home_row_mod_t shortcut_mods[] = {
    {MT(MOD_RCTL, KC_H), MOD_BIT(KC_RIGHT_CTRL)},
    {MT(MOD_RALT, KC_O), MOD_BIT(KC_RIGHT_ALT)},
    {MT(MOD_RGUI, KC_E), MOD_BIT(KC_RIGHT_GUI)},
};

/** Calls of camelCase "get value", then a hold of "<" on Z, then " x" */
static const qmk_call_t symbol_hold_host_calls[] = {
    {CALLED_TAP_CODE16, KC_G},
    {CALLED_TAP_CODE16, KC_E},
    {CALLED_TAP_CODE16, KC_T},
    {CALLED_TAP_CODE16, LSFT(KC_V)},
    {CALLED_TAP_CODE16, KC_A},
    {CALLED_TAP_CODE16, KC_L},
    {CALLED_TAP_CODE16, KC_U},
    {CALLED_TAP_CODE16, KC_E},
    {CALLED_TAP_CODE16, KC_LEFT_ANGLE_BRACKET},
    {CALLED_TAP_CODE16, KC_RIGHT_ANGLE_BRACKET},
    {CALLED_TAP_CODE16, KC_LEFT},
    {CALLED_TAP_CODE16, KC_SPACE},
    {CALLED_TAP_CODE16, KC_X},
};

// #endregion

// #region Static functions

/**
 * Gives the key of a character that is not a letter or a digit.
 *
 * @param character ' ', '\b' or '_'.
 * @return Keycode of the character.
 */
static uint16_t keycode_of_other_character(char character) {
    for (size_t index = 0; index < ARRAY_SIZE(character_keys); index++) {
        if (character_keys[index].character == character) {
            return character_keys[index].keycode;
        }
    }
    fail_msg("No key for character %d", character);
    return KC_NO;
}

/**
 * Gives the key that types a character of a test text, with Shift for a capital.
 *
 * @param character Letter, digit, ' ', '\b' or '_'.
 * @return Keycode of the character.
 */
static uint16_t keycode_of_character(char character) {
    if (islower((unsigned char)character)) {
        return (uint16_t)(KC_A + (character - 'a'));
    } else if (isupper((unsigned char)character)) {
        return (uint16_t)LSFT(KC_A + (character - 'A'));
    } else if (character == '0') {
        return KC_0;
    } else if (isdigit((unsigned char)character)) {
        return (uint16_t)(KC_1 + (character - '1'));
    }
    return keycode_of_other_character(character);
}

/**
 * Pushes and releases one key through process_record_user().
 *
 * @param keycode Keycode that QMK gives the hook.
 * @param is_tap  A_TAP for the tap of a tap-hold key, NOT_A_TAP else.
 * @return Hook result for the push.
 */
static bool push_key(uint16_t keycode, bool is_tap) {
    keyrecord_t push    = {.event = {.pressed = true}};
    keyrecord_t release = {.event = {.pressed = false}};
    // QMK gives both events of a tap the same tap count
    push.tap.count    = is_tap ? ONE_TAP : NO_TAP;
    release.tap.count = push.tap.count;
    bool passes       = process_record_user(keycode, &push);
    assert_true(process_record_user(keycode, &release));
    return passes;
}

/**
 * Types each character of a text with its key. A key that passes goes to the call
 * log as a tap, as QMK sends it.
 *
 * @param text Lower-case letters, digits, ' ' and '\b'.
 */
static void type_text(const char *text) {
    for (size_t index = 0; text[index] != '\0'; index++) {
        uint16_t keycode = keycode_of_character(text[index]);
        if (push_key(keycode, NOT_A_TAP)) {
            tap_code16(keycode);
        }
    }
}

/**
 * Fails the test if the host did not get the keys of a text, in order.
 *
 * @param keys Characters of the keys that the host must get; '\b' for Backspace.
 */
static void assert_host_got(const char *keys) {
    qmk_call_t expected[MOST_HOST_KEYS];
    size_t     count = 0;
    for (; keys[count] != '\0'; count++) {
        assert_true(count < MOST_HOST_KEYS);
        expected[count] = (qmk_call_t){
            .function = CALLED_TAP_CODE16,
            .argument = keycode_of_character(keys[count]),
        };
    }
    assert_recorded_calls(expected, count);
}

/**
 * Empties the call log, then holds a hold key past its tapping term and releases
 * it. The hold sends its held key itself, so no event goes on to QMK.
 *
 * @param keycode Keycode of the hold key.
 */
static void hold_key(uint16_t keycode) {
    reset_fake_qmk();
    keyrecord_t push    = {.event = {.pressed = true}};
    keyrecord_t release = {.event = {.pressed = false}};
    assert_false(process_record_user(keycode, &push));
    assert_false(process_record_user(keycode, &release));
}

/**
 * Fails the test if the host did not get only a push and a release of the held
 * key. Then empties the call log.
 *
 * @param held_key Key that the hold must push and release.
 */
static void assert_host_held(uint16_t held_key) {
    const qmk_call_t expected[] = {
        {CALLED_REGISTER_CODE16, held_key},
        {CALLED_UNREGISTER_CODE16, held_key},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
    reset_fake_qmk();
}

/** Clears the fake, ends any mode and removes any pair that an earlier case left */
static void start_case(void) {
    reset_fake_qmk();
    case_modes_end_all();
    bracket_pairs_clear();
}

static void case_modes_r10_no_mode_at_power_up(void **state) {
    (void)state;
    // Runs first and does not call start_case(), so the state is the power-up state
    reset_fake_qmk();
    type_text("a b");
    assert_host_got("a b");
}

static void case_modes_r1_r2_ac1_snake_case(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max retry count");
    assert_host_got("max_retry_count");
}

static void case_modes_r2_ac2_constant_case(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CONSTANT_CASE_KEY, NOT_A_TAP));
    type_text("max retry");
    assert_host_got("MAX_RETRY");
}

static void case_modes_r2_ac3_camel_case(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("max retry count");
    assert_host_got("maxRetryCount");
}

static void case_modes_r2_ac4_pascal_case(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(PASCAL_CASE_KEY, NOT_A_TAP));
    type_text("max retry count");
    assert_host_got("MaxRetryCount");
}

static void case_modes_r7_ac9_same_mode_key_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(PASCAL_CASE_KEY, NOT_A_TAP));
    assert_false(push_key(PASCAL_CASE_KEY, NOT_A_TAP));
    type_text("a b");
    assert_host_got("a b");
}

static void case_modes_r8_ac10_other_mode_key_changes_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    assert_false(push_key(CONSTANT_CASE_KEY, NOT_A_TAP));
    type_text(" count");
    assert_host_got("max_COUNT");
}

static void case_modes_ac12_no_mode_types_as_is(void **state) {
    (void)state;
    start_case();
    type_text("a b");
    assert_host_got("a b");
}

static void case_modes_end_all_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    case_modes_end_all();
    type_text(" count");
    assert_host_got("max count");
}

static void case_modes_r5_ac5_double_space_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max retry  x");
    // Second Space deletes the underscore, then passes; the editor shows "max_retry x"
    assert_host_got("max_retry_\b x");
}

static void case_modes_r3_ac7_digit_passes(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    // Underscore after the digit shows that the mode is still on
    type_text("item 2 x");
    assert_host_got("item_2_x");
}

static void case_modes_r4_ac8_backspace_keeps_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    // Editor shows "max_rex", then the underscore shows that the mode is still on
    type_text("max ret\bx y");
    assert_host_got("max_ret\bx_y");
}

static void case_modes_r6_ac6_symbol_hold_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("get value");
    // Hold of a bottom-row bracket ends the mode; the bracket pairs send "<>" and Left
    keyrecord_t hold = {.event = {.pressed = true}};
    assert_false(process_record_user(LT(0, KC_Z), &hold));
    hold.event.pressed = false;
    assert_false(process_record_user(LT(0, KC_Z), &hold));
    type_text(" x");
    assert_recorded_calls(symbol_hold_host_calls, ARRAY_SIZE(symbol_hold_host_calls));
}

static void case_modes_r6_function_key_hold_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    assert_host_got("max");
    // Hold of L (1,4) sends F4
    hold_key(LT(0, KC_L));
    assert_host_held(KC_F4);
    type_text(" x");
    assert_host_got(" x");
}

static void case_modes_r6_escape_hold_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    assert_host_got("max");
    // Hold of Tab (1,0) sends Esc
    hold_key(LT(0, KC_TAB));
    assert_host_held(KC_ESCAPE);
    type_text(" x");
    assert_host_got(" x");
}

static void case_modes_mode_key_hold_sends_its_function_key(void **state) {
    (void)state;
    start_case();
    hold_key(SNAKE_CASE_KEY);
    assert_host_held(KC_F6);
    hold_key(CAMEL_CASE_KEY);
    assert_host_held(KC_F7);
    // No hold started a mode
    type_text("a b");
    assert_host_got("a b");
}

static void case_modes_mode_key_hold_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("max");
    hold_key(SNAKE_CASE_KEY);
    assert_host_held(KC_F6);
    type_text(" count");
    assert_host_got(" count");
}

/**
 * Pushes and releases the special mode key, then empties the call log. Special mode
 * takes both events, and its layer calls send no key to the host.
 *
 * @param is_tap A_TAP for a tap, NOT_A_TAP for a hold.
 */
static void push_special_mode_key(bool is_tap) {
    keyrecord_t record = {.event = {.pressed = true}};
    record.tap.count   = is_tap ? ONE_TAP : NO_TAP;
    assert_false(process_record_user(SPECIAL_MODE_KEY, &record));
    record.event.pressed = false;
    assert_false(process_record_user(SPECIAL_MODE_KEY, &record));
    reset_fake_qmk();
}

static void case_modes_special_mode_key_tap_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    assert_host_got("max");
    push_special_mode_key(A_TAP);
    // Fake layer state stays on the base layer, so the Space is a Space
    type_text(" count");
    assert_host_got(" count");
}

static void case_modes_special_mode_key_hold_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    type_text("max");
    assert_host_got("max");
    push_special_mode_key(NOT_A_TAP);
    type_text(" count");
    assert_host_got(" count");
}

static void case_modes_r2_tap_of_top_row_hold_key_is_letter(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("max ");
    // Tap of G (1,2), whose hold sends F2, gives a capital after the Space
    assert_false(push_key(LT(0, KC_G), A_TAP));
    assert_host_got("maxG");
}

static void case_modes_r6_shifted_digit_ends_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("get value");
    // Shift with 9 types "(", a symbol and not a digit
    set_fake_modifiers(MOD_BIT(KC_LEFT_SHIFT));
    assert_true(push_key(KC_9, NOT_A_TAP));
    set_fake_modifiers(0);
    type_text(" x");
    assert_host_got("getValue x");
}

static void case_modes_r9_modifier_keeps_mode(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("max");
    // Hold of the N home row mod, then the Shift key: modifiers alone type nothing
    assert_true(push_key(MT(MOD_LSFT, KC_N), NOT_A_TAP));
    assert_true(push_key(KC_LEFT_SHIFT, NOT_A_TAP));
    type_text(" count");
    assert_host_got("maxCount");
}

static void case_modes_r9_ac11_shortcut_ends_mode(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(shortcut_mods); index++) {
        start_case();
        assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
        assert_true(push_key(shortcut_mods[index].keycode, NOT_A_TAP));
        // QMK sets the modifier of the hold, then sends the chord with S
        set_fake_modifiers(shortcut_mods[index].modifier);
        assert_true(push_key(KC_S, NOT_A_TAP));
        set_fake_modifiers(0);
        type_text(" x");
        assert_host_got(" x");
    }
}

static void case_modes_r2_tap_of_tap_hold_key_is_letter(void **state) {
    (void)state;
    start_case();
    assert_false(push_key(CAMEL_CASE_KEY, A_TAP));
    type_text("max ");
    // Taps of a home row mod and of a bottom-row symbol key give their letters
    assert_false(push_key(MT(MOD_LSFT, KC_N), A_TAP));
    type_text(" ");
    assert_false(push_key(LT(0, KC_X), A_TAP));
    assert_true(push_key(MT(MOD_LCTL, KC_D), A_TAP));
    // QMK sends the tap key of a tap-hold key that passes
    tap_code16(KC_D);
    assert_host_got("maxNXd");
}

static void case_modes_sent_keys_count_in_bracket_pairs(void **state) {
    (void)state;
    start_case();
    // Hold of the "(" key, then snake_case with a double Space in the pair
    keyrecord_t hold = {.event = {.pressed = true}};
    assert_false(process_record_user(LT(0, KC_V), &hold));
    hold.event.pressed = false;
    assert_false(process_record_user(LT(0, KC_V), &hold));
    reset_fake_qmk();
    assert_false(push_key(SNAKE_CASE_KEY, A_TAP));
    // Pair has "ab " after the double Space, so the fourth Backspace deletes the pair
    type_text("ab  \b\b\b");
    keyrecord_t backspace = {.event = {.pressed = true}};
    assert_false(process_record_user(KC_BACKSPACE, &backspace));
    backspace.event.pressed = false;
    assert_false(process_record_user(KC_BACKSPACE, &backspace));
    static const qmk_call_t expected[] = {
        {CALLED_TAP_CODE16, KC_A},          {CALLED_TAP_CODE16, KC_B},
        {CALLED_TAP_CODE16, KC_UNDERSCORE}, {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_SPACE},      {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},  {CALLED_TAP_CODE16, KC_BACKSPACE},
        {CALLED_TAP_CODE16, KC_BACKSPACE},  {CALLED_TAP_CODE16, KC_DELETE},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

// #endregion

// #region Test runner

/** Cases of this program; the power-up case must run first */
static const struct CMUnitTest case_modes_cases[] = {
    cmocka_unit_test(case_modes_r10_no_mode_at_power_up),
    cmocka_unit_test(case_modes_r1_r2_ac1_snake_case),
    cmocka_unit_test(case_modes_r2_ac2_constant_case),
    cmocka_unit_test(case_modes_r2_ac3_camel_case),
    cmocka_unit_test(case_modes_r2_ac4_pascal_case),
    cmocka_unit_test(case_modes_r2_tap_of_tap_hold_key_is_letter),
    cmocka_unit_test(case_modes_r3_ac7_digit_passes),
    cmocka_unit_test(case_modes_r4_ac8_backspace_keeps_mode),
    cmocka_unit_test(case_modes_r5_ac5_double_space_ends_mode),
    cmocka_unit_test(case_modes_r6_ac6_symbol_hold_ends_mode),
    cmocka_unit_test(case_modes_r6_function_key_hold_ends_mode),
    cmocka_unit_test(case_modes_r6_escape_hold_ends_mode),
    cmocka_unit_test(case_modes_mode_key_hold_sends_its_function_key),
    cmocka_unit_test(case_modes_mode_key_hold_ends_mode),
    cmocka_unit_test(case_modes_special_mode_key_tap_ends_mode),
    cmocka_unit_test(case_modes_special_mode_key_hold_ends_mode),
    cmocka_unit_test(case_modes_r2_tap_of_top_row_hold_key_is_letter),
    cmocka_unit_test(case_modes_r6_shifted_digit_ends_mode),
    cmocka_unit_test(case_modes_r9_modifier_keeps_mode),
    cmocka_unit_test(case_modes_r9_ac11_shortcut_ends_mode),
    cmocka_unit_test(case_modes_r7_ac9_same_mode_key_ends_mode),
    cmocka_unit_test(case_modes_r8_ac10_other_mode_key_changes_mode),
    cmocka_unit_test(case_modes_ac12_no_mode_types_as_is),
    cmocka_unit_test(case_modes_end_all_ends_mode),
    cmocka_unit_test(case_modes_sent_keys_count_in_bracket_pairs),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(case_modes_cases, NULL, NULL);
}

// #endregion
