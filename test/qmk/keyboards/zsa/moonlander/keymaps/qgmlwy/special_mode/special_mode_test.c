/** @file
 * Tests of the special mode through the keymap's key event hook: the tap and the
 * hold of the mode key, the one-shot, and the end of the mode on a double
 * whitespace key.
 *
 * Fake layer_on() and layer_off() only record the call, so each case sets the layer
 * state that QMK would have.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "special_mode/special_mode.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Layer state while the special mode is on */
#define SPECIAL_MODE_ON ((layer_state_t)(1U << LAYER_SPECIAL))

/** Layer state while the special mode is off: only the base layer */
#define SPECIAL_MODE_OFF ((layer_state_t)0)

/** Tap count of a hold of the mode key, past the tapping term */
#define NO_TAP 0

/** Tap count of a tap of the mode key */
#define ONE_TAP 1

/** Tap count of a second tap of the mode key in the tapping term */
#define SECOND_TAP 2

/** Value of is_tap for a tap of the mode key */
#define A_TAP true

/** Value of is_tap for a hold of the mode key */
#define A_HOLD false

// #endregion

// #region Data

/** Calls of a push that ends the special mode */
static const qmk_call_t mode_end_calls[] = {
    {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
};

/** Calls of a mode key event that starts the special mode */
static const qmk_call_t mode_start_calls[] = {
    {.function = CALLED_LAYER_ON, .argument = LAYER_SPECIAL},
};

// #endregion

// #region Static functions

/**
 * Sends a push or a release of a key to process_record_user().
 *
 * @param keycode Keycode after layer resolution.
 * @param pressed True for a push.
 * @return Result of process_record_user().
 */
static bool send_key_event(uint16_t keycode, bool pressed) {
    keyrecord_t record = {.event = {.pressed = pressed}};
    return process_record_user(keycode, &record);
}

/**
 * Pushes and releases a key.
 *
 * @param keycode Keycode after layer resolution.
 * @return Result of process_record_user() for the push.
 */
static bool tap_key(uint16_t keycode) {
    bool push_result = send_key_event(keycode, true);
    send_key_event(keycode, false);
    return push_result;
}

/**
 * Sends one event of the mode key, and checks that it sends no key.
 *
 * @param pressed True for a push.
 * @param is_tap  A_TAP for a tap, A_HOLD for a hold past the tapping term.
 */
static void send_mode_key_event(bool pressed, bool is_tap) {
    keyrecord_t record = {.event = {.pressed = pressed}};
    record.tap.count   = is_tap ? ONE_TAP : NO_TAP;
    assert_false(process_record_user(SPECIAL_MODE_KEY, &record));
}

/**
 * Pushes and releases the mode key. QMK gives both events the same tap count.
 *
 * @param is_tap A_TAP for a tap, A_HOLD for a hold past the tapping term.
 */
static void push_mode_key(bool is_tap) {
    send_mode_key_event(true, is_tap);
    send_mode_key_event(false, is_tap);
}

/** Clears the fake, with the special mode off */
static void start_with_mode_off(void) {
    reset_fake_qmk();
    layer_state = SPECIAL_MODE_OFF;
}

/**
 * Starts the special mode with the mode key, then empties the call log. A new start
 * clears each count of an earlier case.
 *
 * @param is_tap A_TAP for a one-shot, A_HOLD for a locked mode.
 */
static void start_special_mode(bool is_tap) {
    start_with_mode_off();
    push_mode_key(is_tap);
    reset_fake_qmk();
    layer_state = SPECIAL_MODE_ON;
}

/**
 * Taps each key of a list, and checks that only the last push ends the mode.
 *
 * @param keycodes Keycodes after layer resolution, in order.
 * @param count    Number of keys.
 * @return True if the last push sends its key, false if it ends the mode.
 */
static bool tap_keys(const uint16_t *keycodes, size_t count) {
    bool push_result = true;
    for (size_t index = 0; index < count; index++) {
        assert_true(push_result);
        push_result = tap_key(keycodes[index]);
    }
    return push_result;
}

static void special_mode_r1_mode_key_is_a_tap_hold_key(void **state) {
    (void)state;
    // Tap-hold engine gives the mode key its tapping term and its chordal hold
    assert_true(IS_QK_LAYER_TAP(SPECIAL_MODE_KEY));
    // Special mode takes each tap, so no tap key can reach the host
    assert_int_equal(QK_LAYER_TAP_GET_TAP_KEYCODE(SPECIAL_MODE_KEY), KC_NO);
}

static void special_mode_r1_tap_starts_one_shot(void **state) {
    (void)state;
    start_with_mode_off();
    push_mode_key(A_TAP);
    assert_recorded_calls(mode_start_calls, ARRAY_SIZE(mode_start_calls));
}

static void special_mode_r1_one_shot_ends_after_a_digit(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    assert_true(send_key_event(KC_1, true));
    // Release comes after the end, with the key from QMK's layer cache
    layer_state = SPECIAL_MODE_OFF;
    assert_true(send_key_event(KC_1, false));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r1_one_shot_ends_after_a_symbol(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    assert_true(tap_key(KC_DOLLAR));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r1_one_shot_ends_after_a_space(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    assert_true(tap_key(KC_SPACE));
    layer_state = SPECIAL_MODE_OFF;
    // Second Space is on the base layer, so it does not count
    assert_true(tap_key(KC_SPACE));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r1_one_shot_ends_after_a_disabled_position(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    assert_true(tap_key(KC_NO));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r1_one_shot_ends_after_a_taken_push(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    // Bracket pairs take a backtick push before the special mode gets it
    special_mode_count_taken_push();
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r1_rolled_push_ends_one_shot(void **state) {
    (void)state;
    start_with_mode_off();
    // QMK settles the tap at the release of the mode key. Then it gives the push of
    // the rolled key on the special layer, and then the release of the mode key
    send_mode_key_event(true, A_TAP);
    layer_state = SPECIAL_MODE_ON;
    assert_true(send_key_event(KC_TILDE, true));
    layer_state = SPECIAL_MODE_OFF;
    send_mode_key_event(false, A_TAP);
    assert_true(send_key_event(KC_TILDE, false));
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_ON, .argument = LAYER_SPECIAL},
        {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void special_mode_r1_hold_starts_locked_mode(void **state) {
    (void)state;
    start_with_mode_off();
    push_mode_key(A_HOLD);
    layer_state                      = SPECIAL_MODE_ON;
    static const uint16_t keycodes[] = {KC_1, KC_SPACE, KC_2, KC_NO, KC_DOLLAR};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_start_calls, ARRAY_SIZE(mode_start_calls));
}

static void special_mode_r1_hold_locks_waiting_one_shot(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    push_mode_key(A_HOLD);
    static const uint16_t keycodes[] = {KC_1, KC_2};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_start_calls, ARRAY_SIZE(mode_start_calls));
}

static void special_mode_r1_hold_keeps_locked_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    push_mode_key(A_HOLD);
    assert_true(tap_key(KC_1));
    assert_recorded_calls(mode_start_calls, ARRAY_SIZE(mode_start_calls));
}

static void special_mode_r1_taken_push_keeps_locked_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    special_mode_count_taken_push();
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r1_taken_push_while_mode_is_off(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    layer_state = SPECIAL_MODE_OFF;
    special_mode_count_taken_push();
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r3_whitespace_keys_send_their_output(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_SPACE, KC_ENTER, KC_TAB, KC_BACKSPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_double_space_ends_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_SPACE, KC_SPACE};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_double_enter_ends_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_ENTER, KC_ENTER};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_double_tab_ends_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_TAB, KC_TAB};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_expression_then_double_space(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {
        KC_1, KC_SPACE, KC_PLUS, KC_SPACE, KC_2, KC_SPACE, KC_SPACE,
    };
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_backspace_between_keeps_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_SPACE, KC_BACKSPACE, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_no_count_while_mode_is_off(void **state) {
    (void)state;
    start_with_mode_off();
    static const uint16_t keycodes[] = {KC_SPACE, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_new_mode_starts_a_new_count(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    assert_true(tap_key(KC_SPACE));
    push_mode_key(A_TAP);
    layer_state = SPECIAL_MODE_OFF;
    push_mode_key(A_HOLD);
    layer_state = SPECIAL_MODE_ON;
    assert_true(tap_key(KC_SPACE));
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
        {.function = CALLED_LAYER_ON, .argument = LAYER_SPECIAL},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void special_mode_r5_space_then_enter_keeps_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_SPACE, KC_ENTER};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r6_disabled_position_keeps_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    static const uint16_t keycodes[] = {KC_SPACE, KC_NO, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r7_tap_ends_locked_mode(void **state) {
    (void)state;
    start_special_mode(A_HOLD);
    push_mode_key(A_TAP);
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r7_tap_ends_waiting_one_shot(void **state) {
    (void)state;
    start_special_mode(A_TAP);
    // QMK counts a second tap in the tapping term as tap 2
    keyrecord_t second_tap = {.event = {.pressed = true}, .tap = {.count = SECOND_TAP}};
    assert_false(process_record_user(SPECIAL_MODE_KEY, &second_tap));
    second_tap.event.pressed = false;
    assert_false(process_record_user(SPECIAL_MODE_KEY, &second_tap));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest special_mode_cases[] = {
    cmocka_unit_test(special_mode_r1_mode_key_is_a_tap_hold_key),
    cmocka_unit_test(special_mode_r1_tap_starts_one_shot),
    cmocka_unit_test(special_mode_r1_one_shot_ends_after_a_digit),
    cmocka_unit_test(special_mode_r1_one_shot_ends_after_a_symbol),
    cmocka_unit_test(special_mode_r1_one_shot_ends_after_a_space),
    cmocka_unit_test(special_mode_r1_one_shot_ends_after_a_disabled_position),
    cmocka_unit_test(special_mode_r1_one_shot_ends_after_a_taken_push),
    cmocka_unit_test(special_mode_r1_rolled_push_ends_one_shot),
    cmocka_unit_test(special_mode_r1_hold_starts_locked_mode),
    cmocka_unit_test(special_mode_r1_hold_locks_waiting_one_shot),
    cmocka_unit_test(special_mode_r1_hold_keeps_locked_mode),
    cmocka_unit_test(special_mode_r1_taken_push_keeps_locked_mode),
    cmocka_unit_test(special_mode_r1_taken_push_while_mode_is_off),
    cmocka_unit_test(special_mode_r3_whitespace_keys_send_their_output),
    cmocka_unit_test(special_mode_r4_double_space_ends_mode),
    cmocka_unit_test(special_mode_r4_double_enter_ends_mode),
    cmocka_unit_test(special_mode_r4_double_tab_ends_mode),
    cmocka_unit_test(special_mode_r4_expression_then_double_space),
    cmocka_unit_test(special_mode_r4_backspace_between_keeps_mode),
    cmocka_unit_test(special_mode_r4_no_count_while_mode_is_off),
    cmocka_unit_test(special_mode_r4_new_mode_starts_a_new_count),
    cmocka_unit_test(special_mode_r5_space_then_enter_keeps_mode),
    cmocka_unit_test(special_mode_r6_disabled_position_keeps_mode),
    cmocka_unit_test(special_mode_r7_tap_ends_locked_mode),
    cmocka_unit_test(special_mode_r7_tap_ends_waiting_one_shot),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(special_mode_cases, NULL, NULL);
}

// #endregion
