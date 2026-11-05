/** @file
 * Tap dance outcomes and the finished, reset and on-each-tap callbacks of the
 * qgmlwy keymap's dances.
 */

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "tap_dance/tap_dance.h"

/**
 * Per-dance record of the outcome a tap dance resolved to.
 *
 * The finished callback stores the outcome so the reset callback can release
 * exactly what was registered.
 */
typedef struct {
    /** Never read; present only to preserve the record's memory layout. */
    bool is_press_action;
    /** Outcome from dance_step() for the current sequence, or 0 after reset. */
    uint8_t step;
} dance_tracker_t;

/** Outcomes dance_step() resolves a tap dance to; 0 means none yet. */
enum {
    /** Tapped once, or pressed once and interrupted by another key. */
    SINGLE_TAP = 1,
    /** Pressed once and still held, uninterrupted. */
    SINGLE_HOLD,
    /** Tapped twice. */
    DOUBLE_TAP,
    /** Tapped once, then pressed again and still held. */
    DOUBLE_HOLD,
    /** Pressed twice and interrupted by another key. */
    DOUBLE_SINGLE_TAP,
    /** Any other tap count. */
    MORE_TAPS
};

/** Outcome record of each dance, indexed by its slot. */
static dance_tracker_t dance_state[DANCE_COUNT];

/** Tap count of a dance tapped or held once. */
#define SINGLE_TAP_COUNT 1

/** Tap count of a dance tapped twice, or tapped once and then held. */
#define DOUBLE_TAP_COUNT 2

/**
 * Resolves a tap dance's taps to one outcome.
 *
 * @param state Tap count and key state of the dance.
 * @return The outcome: SINGLE_TAP to DOUBLE_SINGLE_TAP for one or two taps,
 *         MORE_TAPS for any other tap count.
 */
static uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == SINGLE_TAP_COUNT) {
        if (state->interrupted || !state->pressed)
            return SINGLE_TAP;
        else
            return SINGLE_HOLD;
    } else if (state->count == DOUBLE_TAP_COUNT) {
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    return MORE_TAPS;
}

/**
 * Tap count at which an on-each-tap dance starts repeating its single-tap key.
 *
 * Reaching this count sends the key once per tap so far; each further tap
 * sends it once more.
 */
#define REPEAT_TAP_COUNT 3

void on_dance_0(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_LEFT_CTRL);
        tap_code16(KC_LEFT_CTRL);
        tap_code16(KC_LEFT_CTRL);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_LEFT_CTRL);
    }
}

void dance_0_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_0].step = dance_step(state);
    switch (dance_state[DANCE_0].step) {
        case SINGLE_TAP:
            register_code16(KC_LEFT_CTRL);
            break;
        case SINGLE_HOLD:
            register_code16(KC_LEFT_CTRL);
            break;
        case DOUBLE_TAP:
            register_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_LEFT_CTRL);
            register_code16(KC_LEFT_CTRL);
    }
}

void dance_0_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_0].step) {
        case SINGLE_TAP:
            unregister_code16(KC_LEFT_CTRL);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_LEFT_CTRL);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_LEFT_CTRL);
            break;
    }
    dance_state[DANCE_0].step = 0;
}

void dance_1_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_1].step = dance_step(state);
    switch (dance_state[DANCE_1].step) {
        case SINGLE_TAP:
            layer_move(LAYER_NAVIGATION_GATE);
            break;
        case DOUBLE_TAP:
            layer_move(LAYER_SYSTEM);
            break;
        case DOUBLE_SINGLE_TAP:
            layer_move(LAYER_NAVIGATION_GATE);
            break;
    }
}

void dance_1_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_1].step = 0;
}

void on_dance_2(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_LPRN);
    }
}

void dance_2_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_2].step = dance_step(state);
    switch (dance_state[DANCE_2].step) {
        case SINGLE_TAP:
            register_code16(KC_LPRN);
            break;
        case SINGLE_HOLD:
            register_code16(KC_LCBR);
            break;
        case DOUBLE_TAP:
            register_code16(KC_LBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_LPRN);
            register_code16(KC_LPRN);
    }
}

void dance_2_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_2].step) {
        case SINGLE_TAP:
            unregister_code16(KC_LPRN);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_LCBR);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_LBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_LPRN);
            break;
    }
    dance_state[DANCE_2].step = 0;
}

void on_dance_3(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_RPRN);
        tap_code16(KC_RPRN);
        tap_code16(KC_RPRN);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_RPRN);
    }
}

void dance_3_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_3].step = dance_step(state);
    switch (dance_state[DANCE_3].step) {
        case SINGLE_TAP:
            register_code16(KC_RPRN);
            break;
        case SINGLE_HOLD:
            register_code16(KC_RCBR);
            break;
        case DOUBLE_TAP:
            register_code16(KC_RBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_RPRN);
            register_code16(KC_RPRN);
    }
}

void dance_3_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_3].step) {
        case SINGLE_TAP:
            unregister_code16(KC_RPRN);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_RCBR);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_RBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_RPRN);
            break;
    }
    dance_state[DANCE_3].step = 0;
}

void on_dance_4(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN3);
        tap_code16(KC_MS_BTN3);
        tap_code16(KC_MS_BTN3);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN3);
    }
}

void dance_4_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_4].step = dance_step(state);
    switch (dance_state[DANCE_4].step) {
        case SINGLE_TAP:
            register_code16(KC_MS_BTN3);
            break;
        case SINGLE_HOLD:
            register_code16(KC_MS_WH_DOWN);
            break;
        case DOUBLE_TAP:
            register_code16(KC_MS_BTN3);
            register_code16(KC_MS_BTN3);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_MS_WH_LEFT);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_MS_BTN3);
            register_code16(KC_MS_BTN3);
    }
}

void dance_4_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_4].step) {
        case SINGLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_MS_WH_DOWN);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_MS_WH_LEFT);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
    }
    dance_state[DANCE_4].step = 0;
}

void on_dance_5(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN2);
        tap_code16(KC_MS_BTN2);
        tap_code16(KC_MS_BTN2);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN2);
    }
}

void dance_5_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_5].step = dance_step(state);
    switch (dance_state[DANCE_5].step) {
        case SINGLE_TAP:
            register_code16(KC_MS_BTN2);
            break;
        case SINGLE_HOLD:
            register_code16(KC_MS_WH_UP);
            break;
        case DOUBLE_TAP:
            register_code16(KC_MS_BTN2);
            register_code16(KC_MS_BTN2);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_MS_WH_RIGHT);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_MS_BTN2);
            register_code16(KC_MS_BTN2);
    }
}

void dance_5_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_5].step) {
        case SINGLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_MS_WH_UP);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_MS_WH_RIGHT);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
    }
    dance_state[DANCE_5].step = 0;
}

void dance_6_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_6].step = dance_step(state);
    switch (dance_state[DANCE_6].step) {
        case SINGLE_HOLD:
            register_code16(LALT(LCTL(KC_DELETE)));
            break;
        case DOUBLE_TAP:
            register_code16(LALT(LCTL(LSFT(KC_DELETE))));
            break;
        case DOUBLE_HOLD:
            register_code16(KC_SYSTEM_POWER);
            break;
    }
}

void dance_6_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_6].step) {
        case SINGLE_HOLD:
            unregister_code16(LALT(LCTL(KC_DELETE)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LALT(LCTL(LSFT(KC_DELETE))));
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_SYSTEM_POWER);
            break;
    }
    dance_state[DANCE_6].step = 0;
}

void on_dance_7(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_D));
        tap_code16(LCTL(KC_D));
        tap_code16(LCTL(KC_D));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_D));
    }
}

void dance_7_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_7].step = dance_step(state);
    switch (dance_state[DANCE_7].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_D));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_D)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_D));
            register_code16(LCTL(KC_D));
    }
}

void dance_7_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_7].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_D));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_D)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_D));
            break;
    }
    dance_state[DANCE_7].step = 0;
}

void on_dance_8(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_J)));
        tap_code16(LCTL(LSFT(KC_J)));
        tap_code16(LCTL(LSFT(KC_J)));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_J)));
    }
}

void dance_8_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_8].step = dance_step(state);
    switch (dance_state[DANCE_8].step) {
        case SINGLE_TAP:
            register_code16(LCTL(LSFT(KC_J)));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_I)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(LSFT(KC_J)));
            register_code16(LCTL(LSFT(KC_J)));
    }
}

void dance_8_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_8].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_J)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_I)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_J)));
            break;
    }
    dance_state[DANCE_8].step = 0;
}

void on_dance_9(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_RBRC));
        tap_code16(LCTL(KC_RBRC));
        tap_code16(LCTL(KC_RBRC));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_RBRC));
    }
}

void dance_9_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_9].step = dance_step(state);
    switch (dance_state[DANCE_9].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_RBRC));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_LBRC));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_RBRC));
            register_code16(LCTL(KC_RBRC));
    }
}

void dance_9_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_9].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_RBRC));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_LBRC));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_RBRC));
            break;
    }
    dance_state[DANCE_9].step = 0;
}

void on_dance_10(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_R));
        tap_code16(LCTL(KC_R));
        tap_code16(LCTL(KC_R));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_R));
    }
}

void dance_10_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_10].step = dance_step(state);
    switch (dance_state[DANCE_10].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_R));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_R)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_R));
            register_code16(LCTL(KC_R));
    }
}

void dance_10_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_10].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_R));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_R)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_R));
            break;
    }
    dance_state[DANCE_10].step = 0;
}

void on_dance_11(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_TAB)));
        tap_code16(LCTL(LSFT(KC_TAB)));
        tap_code16(LCTL(LSFT(KC_TAB)));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_TAB)));
    }
}

void dance_11_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_11].step = dance_step(state);
    switch (dance_state[DANCE_11].step) {
        case SINGLE_TAP:
            register_code16(LCTL(LSFT(KC_TAB)));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_1));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_PAGE_UP)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(LSFT(KC_TAB)));
            register_code16(LCTL(LSFT(KC_TAB)));
    }
}

void dance_11_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_11].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_TAB)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_1));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_PAGE_UP)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_TAB)));
            break;
    }
    dance_state[DANCE_11].step = 0;
}

void on_dance_12(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_T));
        tap_code16(LCTL(KC_T));
        tap_code16(LCTL(KC_T));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_T));
    }
}

void dance_12_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_12].step = dance_step(state);
    switch (dance_state[DANCE_12].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_T));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_N));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_N)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_T));
            register_code16(LCTL(KC_T));
    }
}

void dance_12_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_12].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_T));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_N));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_N)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_T));
            break;
    }
    dance_state[DANCE_12].step = 0;
}

void on_dance_13(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_TAB));
        tap_code16(LCTL(KC_TAB));
        tap_code16(LCTL(KC_TAB));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_TAB));
    }
}

void dance_13_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_13].step = dance_step(state);
    switch (dance_state[DANCE_13].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_TAB));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_9));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_PGDN)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_TAB));
            register_code16(LCTL(KC_TAB));
    }
}

void dance_13_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_13].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_TAB));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_9));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_PGDN)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_TAB));
            break;
    }
    dance_state[DANCE_13].step = 0;
}

void on_dance_14(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LGUI(KC_F5));
        tap_code16(LGUI(KC_F5));
        tap_code16(LGUI(KC_F5));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LGUI(KC_F5));
    }
}

void dance_14_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_14].step = dance_step(state);
    switch (dance_state[DANCE_14].step) {
        case SINGLE_TAP:
            register_code16(LGUI(KC_F5));
            break;
        case DOUBLE_TAP:
            register_code16(LGUI(LSFT(KC_F5)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LGUI(KC_F5));
            register_code16(LGUI(KC_F5));
    }
}

void dance_14_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_14].step) {
        case SINGLE_TAP:
            unregister_code16(LGUI(KC_F5));
            break;
        case DOUBLE_TAP:
            unregister_code16(LGUI(LSFT(KC_F5)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LGUI(KC_F5));
            break;
    }
    dance_state[DANCE_14].step = 0;
}

void dance_15_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_15].step = dance_step(state);
    switch (dance_state[DANCE_15].step) {
        case DOUBLE_HOLD:
            layer_move(LAYER_BASE);
            break;
    }
}

void dance_15_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_15].step = 0;
}

void dance_16_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_16].step = dance_step(state);
    switch (dance_state[DANCE_16].step) {
        case DOUBLE_HOLD:
            layer_move(LAYER_BASE);
            break;
    }
}

void dance_16_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_16].step = 0;
}
