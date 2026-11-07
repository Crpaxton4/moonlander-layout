/** @file
 * Characterization tests of the tap dances: the keys that each outcome of each dance
 * sends, through the actions of the keymap's `tap_dance_actions`.
 *
 * QMK's tap dance engine is not in the test build. play_dance() calls the callbacks
 * of an action in the order of the engine. Each reset waits before it releases its
 * key, and the mouse dances register their double tap key two times, but release it
 * one time. Expected calls show both.
 */

#include QMK_KEYBOARD_H

#include "keymap_introspection.h"
#include "recorded_calls.h"
#include "tap_dance/tap_dance.h"
#include "unit_test.h"

/** Wait in milliseconds of each dance reset before it releases its key */
#define RESET_WAIT_MILLISECONDS 10

/** Presses of a dance key until the dance finishes */
typedef struct {
    uint8_t taps;        /**< Number of presses of the dance key */
    bool    held;        /**< Last press is still held when the dance finishes */
    bool    interrupted; /**< Press of another key finishes the dance */
} dance_presses_t;

/** One press and release, then the tapping term ends */
static const dance_presses_t single_tap = {.taps = 1};

/** One press, held until the tapping term ends */
static const dance_presses_t single_hold = {.taps = 1, .held = true};

/** Two presses and releases, then the tapping term ends */
static const dance_presses_t double_tap = {.taps = 2};

/** One press and release, then a second press, held until the tapping term ends */
static const dance_presses_t double_hold = {.taps = 2, .held = true};

/** Three presses and releases, then the tapping term ends */
static const dance_presses_t triple_tap = {.taps = 3};

/** One press, held until a press of another key */
static const dance_presses_t interrupted_hold = {
    .taps        = 1,
    .held        = true,
    .interrupted = true,
};

/** Two presses and releases, then a press of another key */
static const dance_presses_t interrupted_double_tap = {.taps = 2, .interrupted = true};

/**
 * Empties the call log, then plays a dance through the callbacks of its action, in
 * the order of QMK's engine: each tap, then finished, then reset.
 *
 * QMK resets a held dance at its release, after the finished callback, so the order
 * is the same for a held dance.
 *
 * @param slot    Slot of the dance in `tap_dance_actions`.
 * @param presses Presses of the dance key.
 */
static void play_dance(uint16_t slot, dance_presses_t presses) {
    reset_fake_qmk();
    tap_dance_action_t *action = tap_dance_get(slot);
    if (action == NULL) {
        fail_msg("No tap dance action in slot %u", (unsigned)slot);
    }
    tap_dance_state_t *dance_state = &action->state;
    for (uint8_t press = 0; press < presses.taps; press++) {
        dance_state->pressed = true;
        dance_state->count++;
        action->fn.on_each_tap(dance_state, action->user_data);
        dance_state->pressed = false;
    }
    dance_state->pressed     = presses.held;
    dance_state->interrupted = presses.interrupted;
    dance_state->finished    = true;
    action->fn.on_dance_finished(dance_state, action->user_data);
    dance_state->pressed = false;
    action->fn.on_reset(dance_state, action->user_data);
    *dance_state = (tap_dance_state_t){0};
}

static void tap_dance_actions_cover_every_slot(void **state) {
    (void)state;
    assert_int_equal(tap_dance_count(), DANCE_COUNT);
}

static void tap_dance_left_brackets_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_LEFT_PAREN},
    };
    play_dance(DANCE_LEFT_BRACKETS, single_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_CURLY_BRACE},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_LEFT_CURLY_BRACE},
    };
    play_dance(DANCE_LEFT_BRACKETS, single_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_double_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_BRACKET},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_LEFT_BRACKET},
    };
    play_dance(DANCE_LEFT_BRACKETS, double_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_double_hold_sends_nothing(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_LEFT_BRACKETS, double_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_triple_tap_repeats(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_TAP_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_TAP_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_TAP_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_LEFT_BRACKETS, triple_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_interrupted_hold_taps(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_LEFT_PAREN},
    };
    play_dance(DANCE_LEFT_BRACKETS, interrupted_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_left_brackets_interrupted_double_tap_taps_twice(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_TAP_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_LEFT_PAREN},
    };
    play_dance(DANCE_LEFT_BRACKETS, interrupted_double_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_brackets_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_RIGHT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_RIGHT_PAREN},
    };
    play_dance(DANCE_RIGHT_BRACKETS, single_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_brackets_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_RIGHT_CURLY_BRACE},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_RIGHT_CURLY_BRACE},
    };
    play_dance(DANCE_RIGHT_BRACKETS, single_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_brackets_double_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_RIGHT_BRACKET},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = KC_RIGHT_BRACKET},
    };
    play_dance(DANCE_RIGHT_BRACKETS, double_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_brackets_double_hold_sends_nothing(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_RIGHT_BRACKETS, double_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_brackets_triple_tap_repeats(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_TAP_CODE16, .argument = KC_RIGHT_PAREN},
        {.function = CALLED_TAP_CODE16, .argument = KC_RIGHT_PAREN},
        {.function = CALLED_TAP_CODE16, .argument = KC_RIGHT_PAREN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_RIGHT_BRACKETS, triple_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_middle_click_or_wheel_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_BUTTON_3},
    };
    play_dance(DANCE_MIDDLE_CLICK_OR_WHEEL, single_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_middle_click_or_wheel_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_WHEEL_DOWN},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_WHEEL_DOWN},
    };
    play_dance(DANCE_MIDDLE_CLICK_OR_WHEEL, single_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_middle_click_or_wheel_double_tap_registers_twice(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_BUTTON_3},
    };
    play_dance(DANCE_MIDDLE_CLICK_OR_WHEEL, double_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_middle_click_or_wheel_double_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_WHEEL_LEFT},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_WHEEL_LEFT},
    };
    play_dance(DANCE_MIDDLE_CLICK_OR_WHEEL, double_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_middle_click_or_wheel_triple_tap_repeats(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_3},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_MIDDLE_CLICK_OR_WHEEL, triple_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_click_or_wheel_tap(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_BUTTON_2},
    };
    play_dance(DANCE_RIGHT_CLICK_OR_WHEEL, single_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_click_or_wheel_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_WHEEL_UP},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_WHEEL_UP},
    };
    play_dance(DANCE_RIGHT_CLICK_OR_WHEEL, single_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_click_or_wheel_double_tap_registers_twice(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_BUTTON_2},
    };
    play_dance(DANCE_RIGHT_CLICK_OR_WHEEL, double_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_click_or_wheel_double_hold(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = QK_MOUSE_WHEEL_RIGHT},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
        {.function = CALLED_UNREGISTER_CODE16, .argument = QK_MOUSE_WHEEL_RIGHT},
    };
    play_dance(DANCE_RIGHT_CLICK_OR_WHEEL, double_hold);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void tap_dance_right_click_or_wheel_triple_tap_repeats(void **state) {
    (void)state;
    static const qmk_call_t expected[] = {
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_TAP_CODE16, .argument = QK_MOUSE_BUTTON_2},
        {.function = CALLED_WAIT_MS, .argument = RESET_WAIT_MILLISECONDS},
    };
    play_dance(DANCE_RIGHT_CLICK_OR_WHEEL, triple_tap);
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

/** Cases of this program, in the order of the dance slots */
static const struct CMUnitTest tap_dance_cases[] = {
    cmocka_unit_test(tap_dance_actions_cover_every_slot),
    cmocka_unit_test(tap_dance_left_brackets_tap),
    cmocka_unit_test(tap_dance_left_brackets_hold),
    cmocka_unit_test(tap_dance_left_brackets_double_tap),
    cmocka_unit_test(tap_dance_left_brackets_double_hold_sends_nothing),
    cmocka_unit_test(tap_dance_left_brackets_triple_tap_repeats),
    cmocka_unit_test(tap_dance_left_brackets_interrupted_hold_taps),
    cmocka_unit_test(tap_dance_left_brackets_interrupted_double_tap_taps_twice),
    cmocka_unit_test(tap_dance_right_brackets_tap),
    cmocka_unit_test(tap_dance_right_brackets_hold),
    cmocka_unit_test(tap_dance_right_brackets_double_tap),
    cmocka_unit_test(tap_dance_right_brackets_double_hold_sends_nothing),
    cmocka_unit_test(tap_dance_right_brackets_triple_tap_repeats),
    cmocka_unit_test(tap_dance_middle_click_or_wheel_tap),
    cmocka_unit_test(tap_dance_middle_click_or_wheel_hold),
    cmocka_unit_test(tap_dance_middle_click_or_wheel_double_tap_registers_twice),
    cmocka_unit_test(tap_dance_middle_click_or_wheel_double_hold),
    cmocka_unit_test(tap_dance_middle_click_or_wheel_triple_tap_repeats),
    cmocka_unit_test(tap_dance_right_click_or_wheel_tap),
    cmocka_unit_test(tap_dance_right_click_or_wheel_hold),
    cmocka_unit_test(tap_dance_right_click_or_wheel_double_tap_registers_twice),
    cmocka_unit_test(tap_dance_right_click_or_wheel_double_hold),
    cmocka_unit_test(tap_dance_right_click_or_wheel_triple_tap_repeats),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(tap_dance_cases, NULL, NULL);
}
