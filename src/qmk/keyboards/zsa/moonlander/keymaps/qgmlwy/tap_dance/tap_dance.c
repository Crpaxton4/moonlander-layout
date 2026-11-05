/** @file
 * QMK callbacks that play every tap dance from its definition in `dances`.
 *
 * The finished callback resolves the taps to an outcome with tap_outcome(), holds
 * that outcome's key or moves to its layer, and records the outcome so the reset
 * callback releases exactly the key that was held.
 */

#include QMK_KEYBOARD_H

#include "tap_dance/tap_dance.h"
#include "tap_dance/tap_outcome.h"

/** Milliseconds every dance reset waits before releasing its key. */
#define RESET_WAIT_MS 10

/** Outcome each dance finished with, indexed by its slot; NO_OUTCOME once reset. */
static uint8_t dance_outcomes[DANCE_COUNT];

/**
 * Slot of a dance, from its definition's place in `dances`.
 *
 * @param dance Definition in `dances`.
 * @return Index of the definition in `dances`.
 */
static size_t dance_slot(const dance_t *dance) {
    return (size_t)(dance - dances);
}

/**
 * Key a keycode dance holds for an outcome.
 *
 * @param dance   Keys of the dance.
 * @param outcome Outcome from tap_outcome().
 * @return The outcome's key, or KC_NO when the outcome holds none.
 */
static uint16_t outcome_keycode(const keycode_dance_t *dance, uint8_t outcome) {
    switch (outcome) {
        case SINGLE_TAP:
        case DOUBLE_SINGLE_TAP:
            return dance->tap;
        case SINGLE_HOLD:
            return dance->hold;
        case DOUBLE_TAP:
            return dance->double_tap;
        case DOUBLE_HOLD:
            return dance->double_hold;
        default:
            return KC_NO;
    }
}

/**
 * Layer a layer dance moves to for an outcome.
 *
 * @param dance   Layers of the dance.
 * @param outcome Outcome from tap_outcome().
 * @return The outcome's layer, or NO_LAYER when the outcome moves to none.
 */
static uint8_t outcome_layer(const layer_dance_t *dance, uint8_t outcome) {
    switch (outcome) {
        case SINGLE_TAP:
        case DOUBLE_SINGLE_TAP:
            return dance->tap;
        case SINGLE_HOLD:
            return dance->hold;
        case DOUBLE_TAP:
            return dance->double_tap;
        case DOUBLE_HOLD:
            return dance->double_hold;
        default:
            return NO_LAYER;
    }
}

/**
 * Holds a keycode dance's key for an outcome.
 *
 * An interrupted double tap taps the key once before holding it.
 *
 * @param dance   Keys of the dance.
 * @param outcome Outcome from tap_outcome().
 */
static void hold_outcome_keycode(const keycode_dance_t *dance, uint8_t outcome) {
    uint16_t keycode = outcome_keycode(dance, outcome);
    if (keycode == KC_NO) {
        return;
    }
    if (outcome == DOUBLE_SINGLE_TAP) {
        tap_code16(keycode);
    }
    register_code16(keycode);
    if (outcome == DOUBLE_TAP && dance->registers_double_tap_twice) {
        register_code16(keycode);
    }
}

/**
 * Releases the key a keycode dance held for an outcome.
 *
 * @param dance   Keys of the dance.
 * @param outcome Outcome the dance finished with.
 */
static void release_outcome_keycode(const keycode_dance_t *dance, uint8_t outcome) {
    uint16_t keycode = outcome_keycode(dance, outcome);
    if (keycode == KC_NO) {
        return;
    }
    unregister_code16(keycode);
}

/**
 * Moves to a layer dance's layer for an outcome.
 *
 * @param dance   Layers of the dance.
 * @param outcome Outcome from tap_outcome().
 */
static void move_to_outcome_layer(const layer_dance_t *dance, uint8_t outcome) {
    uint8_t layer = outcome_layer(dance, outcome);
    if (layer == NO_LAYER) {
        return;
    }
    layer_move(layer);
}

void dance_on_each_tap(tap_dance_state_t *state, void *user_data) {
    const dance_t *dance = user_data;
    if (dance->kind != DANCE_KEYCODES || !dance->keycodes.repeats_on_triple_tap) {
        return;
    }
    uint8_t taps = repeat_taps(state->count);
    for (uint8_t tap = 0; tap < taps; tap++) {
        tap_code16(dance->keycodes.tap);
    }
}

void dance_finished(tap_dance_state_t *state, void *user_data) {
    const dance_t *dance = user_data;
    uint8_t outcome = tap_outcome(state->count, state->interrupted, state->pressed);
    dance_outcomes[dance_slot(dance)] = outcome;
    if (dance->kind == DANCE_LAYERS) {
        move_to_outcome_layer(&dance->layers, outcome);
        return;
    }
    hold_outcome_keycode(&dance->keycodes, outcome);
}

void dance_reset(tap_dance_state_t *state, void *user_data) {
    const dance_t *dance = user_data;
    size_t         slot  = dance_slot(dance);
    wait_ms(RESET_WAIT_MS);
    if (dance->kind == DANCE_KEYCODES) {
        release_outcome_keycode(&dance->keycodes, dance_outcomes[slot]);
    }
    dance_outcomes[slot] = NO_OUTCOME;
}
