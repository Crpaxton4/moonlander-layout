/** @file
 * Tests of the symbol hold keys on the bottom letter row, through the symbol hold
 * handler and the tap-hold callbacks.
 *
 * Bracket pairs replace the hold of a bracket before this handler in the keymap's
 * key event hook, so the cases call the handler itself. bracket_pairs_test.c covers
 * the brackets through the hook. QMK's layer-tap code gives a tap count of 1 to
 * both events of a tap and 0 to both events of a hold. Each case sends one push and
 * one release in that way.
 */

#include QMK_KEYBOARD_H

#include "keymap_introspection.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "symbol_holds/symbol_holds.h"
#include "unit_test.h"

/** Push and release of one key, as QMK's layer-tap code sends them */
typedef struct {
    keyrecord_t push;    /**< Push event */
    keyrecord_t release; /**< Release event */
} key_events_t;

/** Key events that QMK's layer-tap code sees as a tap */
static const key_events_t tap_events = {
    .push    = {.event = {.pressed = true}, .tap = {.count = 1}},
    .release = {.event = {.pressed = false}, .tap = {.count = 1}},
};

/** Key events that QMK's layer-tap code sees as a hold */
static const key_events_t hold_events = {
    .push    = {.event = {.pressed = true}, .tap = {.count = 0}},
    .release = {.event = {.pressed = false}, .tap = {.count = 0}},
};

/** One symbol hold key of the spec table */
typedef struct {
    uint8_t  row;    /**< Matrix row of the key */
    uint8_t  column; /**< Matrix column of the key */
    uint16_t symbol; /**< Symbol of a hold */
    char     hand;   /**< Hand of the key for chordal hold */
} symbol_hold_case_t;

/** Symbol hold keys from the left pinky to the right pinky, as the spec gives them */
static const symbol_hold_case_t symbol_hold_keys[] = {
    {
        .row    = LEFT_BOTTOM_LETTER_ROW,
        .column = 1,
        .symbol = KC_LEFT_ANGLE_BRACKET,
        .hand   = 'L',
    },
    {
        .row    = LEFT_BOTTOM_LETTER_ROW,
        .column = 2,
        .symbol = KC_LEFT_BRACKET,
        .hand   = 'L',
    },
    {
        .row    = LEFT_BOTTOM_LETTER_ROW,
        .column = 3,
        .symbol = KC_LEFT_CURLY_BRACE,
        .hand   = 'L',
    },
    {
        .row    = LEFT_BOTTOM_LETTER_ROW,
        .column = 4,
        .symbol = KC_LEFT_PAREN,
        .hand   = 'L',
    },
    {
        .row    = LEFT_BOTTOM_LETTER_ROW,
        .column = 5,
        .symbol = KC_UNDERSCORE,
        .hand   = 'L',
    },
    {
        .row    = RIGHT_BOTTOM_LETTER_ROW,
        .column = 1,
        .symbol = KC_MINUS,
        .hand   = 'R',
    },
    {
        .row    = RIGHT_BOTTOM_LETTER_ROW,
        .column = 2,
        .symbol = KC_RIGHT_PAREN,
        .hand   = 'R',
    },
    {
        .row    = RIGHT_BOTTOM_LETTER_ROW,
        .column = 3,
        .symbol = KC_RIGHT_CURLY_BRACE,
        .hand   = 'R',
    },
    {
        .row    = RIGHT_BOTTOM_LETTER_ROW,
        .column = 4,
        .symbol = KC_RIGHT_BRACKET,
        .hand   = 'R',
    },
    {
        .row    = RIGHT_BOTTOM_LETTER_ROW,
        .column = 5,
        .symbol = KC_RIGHT_ANGLE_BRACKET,
        .hand   = 'R',
    },
};

/**
 * Gives the base layer keycode of a symbol hold key.
 *
 * @param key Symbol hold key.
 * @return Keycode at the position of the key.
 */
static uint16_t keycode_of(const symbol_hold_case_t *key) {
    return keycode_at_keymap_location(LAYER_BASE, key->row, key->column);
}

/**
 * Empties the call log, then sends a push and a release of a key to
 * process_symbol_holds().
 *
 * @param keycode Keycode of the key.
 * @param events  Push and release events.
 * @return Result of process_symbol_holds() for the push; the release must give the
 *         same result.
 */
static bool send_key(uint16_t keycode, key_events_t events) {
    reset_fake_qmk();
    bool push_result    = process_symbol_holds(keycode, &events.push);
    bool release_result = process_symbol_holds(keycode, &events.release);
    assert_int_equal(push_result, release_result);
    return push_result;
}

static void symbol_holds_r2_r3_hold_pushes_and_releases_its_symbol(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        const symbol_hold_case_t *key = &symbol_hold_keys[index];
        assert_false(send_key(keycode_of(key), hold_events));
        const qmk_call_t expected[] = {
            {.function = CALLED_REGISTER_CODE16, .argument = key->symbol},
            {.function = CALLED_UNREGISTER_CODE16, .argument = key->symbol},
        };
        assert_recorded_calls(expected, ARRAY_SIZE(expected));
    }
}

static void symbol_holds_r1_tap_sends_the_usual_key(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        assert_true(send_key(keycode_of(&symbol_hold_keys[index]), tap_events));
        assert_recorded_calls(NULL, 0);
    }
}

static void symbol_holds_r2_symbol_of_each_hold_key(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        const symbol_hold_case_t *key = &symbol_hold_keys[index];
        assert_int_equal(symbol_of_hold_key(keycode_of(key)), key->symbol);
    }
}

static void symbol_holds_r2_hold_term(void **state) {
    (void)state;
    keyrecord_t push = hold_events.push;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        uint16_t keycode = keycode_of(&symbol_hold_keys[index]);
        assert_int_equal(get_tapping_term(keycode, &push), TAPPING_TERM);
    }
}

static void symbol_holds_r4_r5_hand_of_each_key(void **state) {
    (void)state;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        const symbol_hold_case_t *key      = &symbol_hold_keys[index];
        keypos_t                  position = {.row = key->row, .col = key->column};
        assert_int_equal(chordal_hold_handedness(position), key->hand);
    }
}

/** Index of the symbol hold key to compare when the prior key is the first one */
#define SECOND_SYMBOL_HOLD_KEY 1

/**
 * Gives the flow tap term of the symbol hold keys after a prior key. Fails the test
 * if two symbol hold keys have different terms. The prior key itself is not
 * compared, because a push again of the same key has its own rule (R8).
 *
 * @param prior_key Keycode of the key before the symbol hold key.
 * @return Flow tap term of each other symbol hold key.
 */
static uint16_t symbol_hold_flow_tap_term_after(uint16_t prior_key) {
    keyrecord_t push      = hold_events.push;
    uint16_t    other_key = keycode_of(&symbol_hold_keys[0]);
    if (other_key == prior_key) {
        other_key = keycode_of(&symbol_hold_keys[SECOND_SYMBOL_HOLD_KEY]);
    }
    uint16_t first_term = get_flow_tap_term(other_key, &push, prior_key);
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        uint16_t keycode = keycode_of(&symbol_hold_keys[index]);
        if (keycode != prior_key) {
            assert_int_equal(get_flow_tap_term(keycode, &push, prior_key), first_term);
        }
    }
    return first_term;
}

static void symbol_holds_r8_no_flow_tap_on_a_push_of_the_same_key(void **state) {
    (void)state;
    keyrecord_t push = hold_events.push;
    for (size_t index = 0; index < ARRAY_SIZE(symbol_hold_keys); index++) {
        uint16_t keycode = keycode_of(&symbol_hold_keys[index]);
        assert_int_equal(get_flow_tap_term(keycode, &push, keycode), 0);
    }
}

static void symbol_holds_r6_flow_tap_term_after_a_letter(void **state) {
    (void)state;
    assert_int_equal(symbol_hold_flow_tap_term_after(KC_E), FLOW_TAP_TERM);
    assert_int_equal(symbol_hold_flow_tap_term_after(SYMBOL_HOLD(KC_V)), FLOW_TAP_TERM);
    assert_int_equal(symbol_hold_flow_tap_term_after(KC_SPACE), 0);
    assert_int_equal(symbol_hold_flow_tap_term_after(SYMBOL_HOLD(KC_COMMA)), 0);
}

static void symbol_holds_r9_flow_tap_while_ctrl_is_held(void **state) {
    (void)state;
    reset_fake_qmk();
    set_fake_modifiers(MOD_BIT(KC_LEFT_CTRL));
    assert_int_equal(symbol_hold_flow_tap_term_after(KC_E), FLOW_TAP_TERM);
}

static void symbol_holds_r9_hold_while_ctrl_is_held(void **state) {
    (void)state;
    reset_fake_qmk();
    set_fake_modifiers(MOD_BIT(KC_LEFT_CTRL));
    keyrecord_t push = hold_events.push;
    assert_false(process_record_user(SYMBOL_HOLD(KC_Z), &push));
    static const qmk_call_t expected[] = {
        {.function = CALLED_REGISTER_CODE16, .argument = KC_LEFT_ANGLE_BRACKET},
    };
    assert_recorded_calls(expected, ARRAY_SIZE(expected));
}

static void symbol_holds_other_keys_have_no_symbol(void **state) {
    (void)state;
    assert_int_equal(symbol_of_hold_key(KC_Z), KC_NO);
    assert_int_equal(symbol_of_hold_key(LCTL_T(KC_D)), KC_NO);
    assert_int_equal(symbol_of_hold_key(LT(LAYER_ARROWS, KC_Z)), KC_NO);
}

static void symbol_holds_other_keys_pass_on(void **state) {
    (void)state;
    assert_true(send_key(KC_Z, hold_events));
    assert_true(send_key(LCTL_T(KC_D), hold_events));
    assert_recorded_calls(NULL, 0);
}

static void symbol_holds_r4_shift_of_a_symbol_ends_at_the_next_push(void **state) {
    (void)state;
    // QMK can process a buffered push after a hold settles, so the weak Shift of
    // "{" must not reach the next key: hold C, tap E gives "{e"
    reset_fake_qmk();
    add_weak_mods(MOD_BIT(KC_LEFT_SHIFT));
    keyrecord_t push = {.event = {.pressed = true}};
    process_record_user(KC_E, &push);
    assert_int_equal(get_weak_mods(), 0);
}

static void symbol_holds_r7_release_keeps_the_shift_of_a_held_symbol(void **state) {
    (void)state;
    reset_fake_qmk();
    add_weak_mods(MOD_BIT(KC_LEFT_SHIFT));
    keyrecord_t release = {.event = {.pressed = false}};
    process_record_user(KC_E, &release);
    assert_int_equal(get_weak_mods(), MOD_BIT(KC_LEFT_SHIFT));
}

/** Cases of this program, in the order of the spec requirements */
static const struct CMUnitTest symbol_holds_cases[] = {
    cmocka_unit_test(symbol_holds_r1_tap_sends_the_usual_key),
    cmocka_unit_test(symbol_holds_r2_r3_hold_pushes_and_releases_its_symbol),
    cmocka_unit_test(symbol_holds_r2_symbol_of_each_hold_key),
    cmocka_unit_test(symbol_holds_r2_hold_term),
    cmocka_unit_test(symbol_holds_r4_r5_hand_of_each_key),
    cmocka_unit_test(symbol_holds_r4_shift_of_a_symbol_ends_at_the_next_push),
    cmocka_unit_test(symbol_holds_r6_flow_tap_term_after_a_letter),
    cmocka_unit_test(symbol_holds_r7_release_keeps_the_shift_of_a_held_symbol),
    cmocka_unit_test(symbol_holds_r8_no_flow_tap_on_a_push_of_the_same_key),
    cmocka_unit_test(symbol_holds_r9_flow_tap_while_ctrl_is_held),
    cmocka_unit_test(symbol_holds_r9_hold_while_ctrl_is_held),
    cmocka_unit_test(symbol_holds_other_keys_have_no_symbol),
    cmocka_unit_test(symbol_holds_other_keys_pass_on),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(symbol_holds_cases, NULL, NULL);
}
