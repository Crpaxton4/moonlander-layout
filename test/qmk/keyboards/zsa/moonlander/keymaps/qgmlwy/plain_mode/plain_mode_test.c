/** @file
 * Tests of the plain mode through the keymap's key event hook: the plain-mode combo
 * and the record handlers that must not operate in the plain mode.
 *
 * Fake layer_on() and layer_off() only record the call, so each case sets the layer
 * state that QMK would have.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "keymap_introspection.h"
#include "mousekey.h"

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "combos/combos.h"
#include "fake_state.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "symbol_holds/symbol_holds.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Layer state with one layer on */
#define ONLY_LAYER(layer) ((layer_state_t)(1U << (layer)))

/** Layer state of the normal mode: only the base layer */
#define NORMAL_MODE ((layer_state_t)0)

/** Layer state of the plain mode */
#define PLAIN_MODE ONLY_LAYER(LAYER_PLAIN)

/** Top cursor speed of the keymap, in counts per movement report */
#define TOP_SPEED 20

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
 * Empties the call log and sets a layer state, with no case mode and no open bracket
 * pair from an earlier case.
 *
 * @param state Layer state that QMK has.
 */
static void start_with_layers(layer_state_t state) {
    reset_fake_qmk();
    case_modes_end_all();
    bracket_pairs_clear();
    layer_state = state;
}

static void plain_mode_r1_combo_keys(void **state) {
    (void)state;
    const combo_t *combo = combo_get(PLAIN_MODE_COMBO);
    assert_int_equal(combo->keys[0], PLAIN_MODE_COMBO_LEFT_POSITION);
    assert_int_equal(combo->keys[1], PLAIN_MODE_COMBO_RIGHT_POSITION);
    assert_int_equal(combo->keys[2], COMBO_END);
    assert_int_equal(combo->keycode, PLAIN_MODE_TOGGLE);
}

static void plain_mode_r1_r2_combo_starts_plain_mode(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    assert_false(send_key_event(PLAIN_MODE_TOGGLE, true));
    // layer_on() does not write the EEPROM, so a power-up starts the normal mode
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_OFF, .argument = LAYER_SPECIAL},
        {.function = CALLED_LAYER_ON, .argument = LAYER_PLAIN},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void plain_mode_r1_ac8_combo_ends_plain_mode(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    assert_false(send_key_event(PLAIN_MODE_TOGGLE, true));
    static const qmk_call_t expected[] = {
        {.function = CALLED_LAYER_OFF, .argument = LAYER_PLAIN},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void plain_mode_r7_combo_release_sends_no_key(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    assert_false(send_key_event(PLAIN_MODE_TOGGLE, false));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_special_mode_r10_combo_does_nothing(void **state) {
    (void)state;
    start_with_layers(ONLY_LAYER(LAYER_SPECIAL));
    assert_false(tap_key(PLAIN_MODE_TOGGLE));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_start_ends_the_case_mode(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    tap_key(SNAKE_CASE_KEY);
    assert_true(tap_key(KC_A));
    tap_key(PLAIN_MODE_TOGGLE);
    // Back to the normal mode, which has only the base layer
    reset_fake_qmk();
    // Snake case would send an underscore in place of the Space
    assert_true(tap_key(KC_SPACE));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_start_clears_the_bracket_pairs(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    // Hold of "(" opens a pair, so a Tab would jump out of it
    tap_key(SYMBOL_HOLD(KC_V));
    tap_key(PLAIN_MODE_TOGGLE);
    // Back to the normal mode, which has only the base layer
    reset_fake_qmk();
    assert_true(tap_key(KC_TAB));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_r5_bracket_pairs_do_not_operate(void **state) {
    (void)state;
    // Spec 33 R15: the guard gives no push to the bracket pairs in the plain mode,
    // so Shift with the quote key types one double quote, and Tab is a Tab
    start_with_layers(PLAIN_MODE);
    set_fake_modifiers(MOD_BIT(KC_LEFT_SHIFT));
    assert_true(tap_key(KC_QUOTE));
    set_fake_modifiers(0);
    assert_true(tap_key(KC_TAB));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_r5_case_modes_do_not_operate(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    assert_true(tap_key(SNAKE_CASE_KEY));
    assert_true(tap_key(KC_A));
    assert_true(tap_key(KC_SPACE));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_r5_special_mode_does_not_operate(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    assert_true(tap_key(SPECIAL_MODE_KEY));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_r5_precision_key_does_not_operate(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    mk_max_speed = TOP_SPEED;
    assert_true(send_key_event(QK_MOUSE_ACCELERATION_0, true));
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

static void plain_mode_r3_symbol_hold_does_not_operate(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    // Tap count 0 is a hold
    assert_true(send_key_event(SYMBOL_HOLD(KC_Z), true));
    assert_recorded_calls(NULL, 0);
}

static void plain_mode_r6_logging_combo_operates(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    bool logging_was_on = key_event_log_is_on();
    assert_false(tap_key(LOGGING_TOGGLE));
    assert_int_not_equal(key_event_log_is_on(), logging_was_on);
}

static void plain_mode_r6_log_records_the_plain_layer(void **state) {
    (void)state;
    start_with_layers(PLAIN_MODE);
    if (!key_event_log_is_on()) {
        tap_key(LOGGING_TOGGLE);
    }
    reset_fake_qmk();
    layer_state = PLAIN_MODE;
    send_key_event(KC_A, true);
    assert_console_text("KL,I,0,d,0,0,4,0004,00,00,0,0\n");
}

static void plain_mode_r4_symbol_hold_release_after_mode_start(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    // Tap count 0 is a hold, which pushes the symbol. "_" is not a bracket, so the
    // symbol holds keep it registered until the release
    send_key_event(SYMBOL_HOLD(KC_J), true);
    reset_fake_qmk();
    layer_state = PLAIN_MODE;
    assert_false(send_key_event(SYMBOL_HOLD(KC_J), false));
    uint16_t         symbol     = symbol_of_hold_key(SYMBOL_HOLD(KC_J));
    const qmk_call_t expected[] = {
        {.function = CALLED_UNREGISTER_CODE16, .argument = symbol},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void plain_mode_r4_precision_key_release_after_mode_start(void **state) {
    (void)state;
    start_with_layers(NORMAL_MODE);
    mk_max_speed = TOP_SPEED;
    send_key_event(QK_MOUSE_ACCELERATION_0, true);
    layer_state = PLAIN_MODE;
    send_key_event(QK_MOUSE_ACCELERATION_0, false);
    assert_int_equal(mk_max_speed, TOP_SPEED);
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest plain_mode_cases[] = {
    cmocka_unit_test(plain_mode_r1_combo_keys),
    cmocka_unit_test(plain_mode_r1_r2_combo_starts_plain_mode),
    cmocka_unit_test(plain_mode_r1_ac8_combo_ends_plain_mode),
    cmocka_unit_test(plain_mode_r7_combo_release_sends_no_key),
    cmocka_unit_test(plain_mode_special_mode_r10_combo_does_nothing),
    cmocka_unit_test(plain_mode_start_ends_the_case_mode),
    cmocka_unit_test(plain_mode_start_clears_the_bracket_pairs),
    cmocka_unit_test(plain_mode_r5_bracket_pairs_do_not_operate),
    cmocka_unit_test(plain_mode_r5_case_modes_do_not_operate),
    cmocka_unit_test(plain_mode_r5_special_mode_does_not_operate),
    cmocka_unit_test(plain_mode_r5_precision_key_does_not_operate),
    cmocka_unit_test(plain_mode_r3_symbol_hold_does_not_operate),
    cmocka_unit_test(plain_mode_r4_symbol_hold_release_after_mode_start),
    cmocka_unit_test(plain_mode_r4_precision_key_release_after_mode_start),
    cmocka_unit_test(plain_mode_r6_logging_combo_operates),
    cmocka_unit_test(plain_mode_r6_log_records_the_plain_layer),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(plain_mode_cases, NULL, NULL);
}

// #endregion
