/** @file
 * Characterization tests of the copy and paste keys, through the keymap's key event
 * hook.
 *
 * QMK's layer-tap code gives a tap count of 1 to both events of a tap and 0 to both
 * events of a hold. Each case sends one press and one release in that way.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "recorded_calls.h"
#include "unit_test.h"

/** Press and release of one key, as QMK's layer-tap code sends them */
typedef struct {
    keyrecord_t press;   /**< Press event */
    keyrecord_t release; /**< Release event */
} key_events_t;

/** Key events that QMK's layer-tap code sees as a tap */
static const key_events_t tap_events = {
    .press   = {.event = {.pressed = true}, .tap = {.count = 1}},
    .release = {.event = {.pressed = false}, .tap = {.count = 1}},
};

/** Key events that QMK's layer-tap code sees as a hold */
static const key_events_t hold_events = {
    .press   = {.event = {.pressed = true}, .tap = {.count = 0}},
    .release = {.event = {.pressed = false}, .tap = {.count = 0}},
};

/**
 * Empties the call log, then sends a press and a release of a key to
 * process_record_user().
 *
 * @param keycode Keycode of the key.
 * @param events  Press and release events.
 * @return Result of process_record_user() for the press; the release must give the
 *         same result.
 */
static bool send_key(uint16_t keycode, key_events_t events) {
    reset_fake_qmk();
    bool press_result   = process_record_user(keycode, &events.press);
    bool release_result = process_record_user(keycode, &events.release);
    assert_int_equal(press_result, release_result);
    return press_result;
}

static void dual_function_copy_key_tap(void **state) {
    (void)state;
    assert_false(send_key(COPY_KEY, tap_events));
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = LCTL(KC_C)},
        {.function = CALLED_UNREGISTER_CODE16, .argument = LCTL(KC_C)},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void dual_function_copy_key_hold(void **state) {
    (void)state;
    assert_false(send_key(COPY_KEY, hold_events));
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = LCTL(LSFT(KC_C))},
        {.function = CALLED_UNREGISTER_CODE16, .argument = LCTL(LSFT(KC_C))},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void dual_function_paste_key_tap(void **state) {
    (void)state;
    assert_false(send_key(PASTE_KEY, tap_events));
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = LCTL(KC_V)},
        {.function = CALLED_UNREGISTER_CODE16, .argument = LCTL(KC_V)},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void dual_function_paste_key_hold(void **state) {
    (void)state;
    assert_false(send_key(PASTE_KEY, hold_events));
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = LCTL(LSFT(KC_V))},
        {.function = CALLED_UNREGISTER_CODE16, .argument = LCTL(LSFT(KC_V))},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void dual_function_other_key_passes_on(void **state) {
    (void)state;
    assert_true(send_key(KC_C, tap_events));
    assert_recorded_calls(NULL, 0);
}

static void dual_function_other_layer_tap_key_passes_on(void **state) {
    (void)state;
    assert_true(send_key(LT(0, KC_X), hold_events));
    assert_recorded_calls(NULL, 0);
}

/** Cases of this program */
static const struct CMUnitTest dual_function_cases[] = {
    cmocka_unit_test(dual_function_copy_key_tap),
    cmocka_unit_test(dual_function_copy_key_hold),
    cmocka_unit_test(dual_function_paste_key_tap),
    cmocka_unit_test(dual_function_paste_key_hold),
    cmocka_unit_test(dual_function_other_key_passes_on),
    cmocka_unit_test(dual_function_other_layer_tap_key_passes_on),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(dual_function_cases, NULL, NULL);
}
