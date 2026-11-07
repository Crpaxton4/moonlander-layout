/** @file
 * Tests of the special mode through the keymap's key event hook: the mode key and
 * the end of the mode on a double whitespace key.
 *
 * Fake layer_on() and layer_off() only record the call, so each case sets the layer
 * state that QMK would have.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Layer state while the special mode is on */
#define SPECIAL_MODE_ON ((layer_state_t)(1U << LAYER_SPECIAL))

/** Layer state while the special mode is off: only the base layer */
#define SPECIAL_MODE_OFF ((layer_state_t)0)

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
 * Sets the special mode on or off, then empties the call log.
 *
 * A mode key tap starts each mode on, so no count stays from an earlier case.
 *
 * @param mode_on True for the special mode on.
 */
static void start_with_special_mode(bool mode_on) {
    reset_fake_qmk();
    layer_state = SPECIAL_MODE_OFF;
    if (mode_on) {
        tap_key(SPECIAL_MODE_KEY);
    }
    reset_fake_qmk();
    layer_state = mode_on ? SPECIAL_MODE_ON : SPECIAL_MODE_OFF;
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

/** Calls of a push that ends the special mode */
static const qmk_call_t mode_end_calls[] = {
    {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
};

static void special_mode_r1_mode_key_starts_mode(void **state) {
    (void)state;
    start_with_special_mode(false);
    assert_false(send_key_event(SPECIAL_MODE_KEY, true));
    assert_false(send_key_event(SPECIAL_MODE_KEY, false));
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_ON, .argument = LAYER_SPECIAL},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void special_mode_r7_mode_key_ends_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    assert_false(send_key_event(SPECIAL_MODE_KEY, true));
    assert_false(send_key_event(SPECIAL_MODE_KEY, false));
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void special_mode_r3_whitespace_keys_send_their_output(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_SPACE, KC_ENTER, KC_TAB, KC_BACKSPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_double_space_ends_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_SPACE, KC_SPACE};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_double_enter_ends_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_ENTER, KC_ENTER};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_double_tab_ends_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_TAB, KC_TAB};
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_expression_then_double_space(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {
        KC_1, KC_SPACE, KC_PLUS, KC_SPACE, KC_2, KC_SPACE, KC_SPACE,
    };
    assert_false(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(mode_end_calls, ARRAY_SIZE(mode_end_calls));
}

static void special_mode_r4_backspace_between_keeps_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_SPACE, KC_BACKSPACE, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_no_count_while_mode_is_off(void **state) {
    (void)state;
    start_with_special_mode(false);
    static const uint16_t keycodes[] = {KC_SPACE, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r4_new_mode_starts_a_new_count(void **state) {
    (void)state;
    start_with_special_mode(true);
    assert_true(tap_key(KC_SPACE));
    tap_key(SPECIAL_MODE_KEY);
    layer_state = SPECIAL_MODE_OFF;
    tap_key(SPECIAL_MODE_KEY);
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
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_SPACE, KC_ENTER};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

static void special_mode_r6_disabled_position_keeps_mode(void **state) {
    (void)state;
    start_with_special_mode(true);
    static const uint16_t keycodes[] = {KC_SPACE, KC_NO, KC_SPACE};
    assert_true(tap_keys(keycodes, ARRAY_SIZE(keycodes)));
    assert_recorded_calls(NULL, 0);
}

/** Cases of this program */
static const struct CMUnitTest special_mode_cases[] = {
    cmocka_unit_test(special_mode_r1_mode_key_starts_mode),
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
    cmocka_unit_test(special_mode_r7_mode_key_ends_mode),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(special_mode_cases, NULL, NULL);
}
