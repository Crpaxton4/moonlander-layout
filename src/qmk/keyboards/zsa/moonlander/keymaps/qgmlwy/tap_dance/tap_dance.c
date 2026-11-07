/** @file
 * QMK callbacks that play every tap dance from its definition in `dances`.
 *
 * The finished callback looks the taps up in `outcomes`, holds that outcome's key,
 * and records the held key so the reset callback releases exactly that key.
 */

#include QMK_KEYBOARD_H

#include "tap_dance/tap_dance.h"

/// Milliseconds every dance reset waits before releasing its key
#define RESET_WAIT_MILLISECONDS 10

/// Tap count of a dance tapped or held once
#define SINGLE_TAP_COUNT 1

/// Tap count of a dance tapped twice, or tapped once and then held
#define DOUBLE_TAP_COUNT 2

/// Tap count at which a repeating dance starts repeating its key
#define REPEAT_TAP_COUNT 3

/// Taps a repeating dance sends for each tap after REPEAT_TAP_COUNT
#define TAPS_PER_FURTHER_TAP 1

/**
 * Outcomes a tap dance resolves to, in the order of the keys of a dance_t, so each
 * is the index of its key in the list dance_finished() makes of them.
 */
enum dance_outcome {
    TAPPED,        ///< Tapped once, or pressed once or twice and interrupted
    HELD,          ///< Pressed once and still held, uninterrupted
    DOUBLE_TAPPED, ///< Tapped twice
    DOUBLE_HELD,   ///< Tapped once, then pressed again and still held
    NO_OUTCOME,    ///< Any other tap count, none included: does nothing
};

/// Values a bool takes as an array index: false, then true
#define BOOL_VALUES 2

/**
 * Outcome of each tap count up to DOUBLE_TAP_COUNT, indexed by the count, then by
 * whether another key interrupted the dance, then by whether its key is still held.
 */
static const uint8_t outcomes[DOUBLE_TAP_COUNT + 1][BOOL_VALUES][BOOL_VALUES] = {
    [0]                = {{NO_OUTCOME, NO_OUTCOME}, {NO_OUTCOME, NO_OUTCOME}},
    [SINGLE_TAP_COUNT] = {{TAPPED, HELD}, {TAPPED, TAPPED}},
    [DOUBLE_TAP_COUNT] = {{DOUBLE_TAPPED, DOUBLE_HELD}, {TAPPED, TAPPED}},
};

/// Key each dance's finished callback held, indexed by its slot; KC_NO for none
static uint16_t held_keycodes[DANCE_COUNT];

void dance_on_each_tap(tap_dance_state_t *state, void *user_data) {
    const dance_t *dance = user_data;
    if (!dance->repeats_on_triple_tap || state->count < REPEAT_TAP_COUNT) {
        return;
    }
    uint8_t taps =
        state->count == REPEAT_TAP_COUNT ? REPEAT_TAP_COUNT : TAPS_PER_FURTHER_TAP;
    for (uint8_t tap = 0; tap < taps; tap++) {
        tap_code16(dance->tap);
    }
}

void dance_finished(tap_dance_state_t *state, void *user_data) {
    const dance_t *dance  = user_data;
    uint16_t       keys[] = {
        dance->tap, dance->hold, dance->double_tap, dance->double_hold, KC_NO,
    };
    uint8_t outcome =
        state->count <= DOUBLE_TAP_COUNT
            ? outcomes[state->count][(int)state->interrupted][(int)state->pressed]
            : NO_OUTCOME;
    uint16_t keycode              = keys[outcome];
    held_keycodes[dance - dances] = keycode;
    if (keycode == KC_NO) {
        return;
    }
    if (state->count == DOUBLE_TAP_COUNT && state->interrupted) {
        tap_code16(keycode);
    }
    register_code16(keycode);
    if (outcome == DOUBLE_TAPPED && dance->registers_double_tap_twice) {
        register_code16(keycode);
    }
}

void dance_reset(tap_dance_state_t *state, void *user_data) {
    (void)state;
    const dance_t *dance = user_data;
    size_t         slot  = (size_t)(dance - dances);
    wait_ms(RESET_WAIT_MILLISECONDS);
    if (held_keycodes[slot] != KC_NO) {
        unregister_code16(held_keycodes[slot]);
    }
    held_keycodes[slot] = KC_NO;
}
