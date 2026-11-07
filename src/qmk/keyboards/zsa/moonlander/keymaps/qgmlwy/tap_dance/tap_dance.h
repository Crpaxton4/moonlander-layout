/** @file
 * Tap dance slots of the qgmlwy keymap, the shape of their definitions and the
 * QMK callbacks that play them.
 *
 * Every dance is one row of `dances`, which names the key each outcome holds. One
 * set of callbacks plays every row, and the keymap's `tap_dance_actions` binds each
 * slot to its row with ACTION_TAP_DANCE_TABLE().
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Tap dance slots, each the index of its action in `tap_dance_actions` and of its
 * definition in `dances`.
 */
enum tap_dance_codes {
    DANCE_CTRL_OR_GUI,
    DANCE_LEFT_BRACKETS,
    DANCE_RIGHT_BRACKETS,
    DANCE_MIDDLE_CLICK_OR_WHEEL,
    DANCE_RIGHT_CLICK_OR_WHEEL,
    /** Number of tap dance slots; sizes `dances`. */
    DANCE_COUNT,
};

/**
 * Definition of one tap dance: the key each outcome holds, and two options.
 *
 * KC_NO makes an outcome do nothing. Two presses interrupted by another key tap the
 * tap key once, then hold it.
 */
typedef struct {
    /** Held on a single tap; tapped, then held, on an interrupted double tap. */
    uint16_t tap;
    /** Held while the dance key is held. */
    uint16_t hold;
    /** Held on a double tap. */
    uint16_t double_tap;
    /** Held on a tap followed by a hold. */
    uint16_t double_hold;
    /** Registers the double-tap key twice on a double tap; one release follows. */
    bool registers_double_tap_twice;
    /** Taps the tap key three times on a third tap, then once per further tap. */
    bool repeats_on_triple_tap;
} dance_t;

/** Definitions of the tap dances, indexed by their slot. */
extern const dance_t dances[DANCE_COUNT];

/**
 * QMK tap dance action that plays the definition in `dances` at a slot.
 *
 * QMK hands the definition back to each callback as its `user_data`, which QMK
 * types as mutable; the callbacks only read it.
 */
#define ACTION_TAP_DANCE_TABLE(slot)                                         \
    {                                                                        \
        .fn        = {dance_on_each_tap, dance_finished, dance_reset, NULL}, \
        .user_data = (void *)&dances[(slot)],                                \
    }

/**
 * Repeats a dance's tap key on long tap runs, if its definition asks for that:
 * three times at the third tap, then once per further tap.
 *
 * @param state     Tap count of the dance.
 * @param user_data The dance's definition in `dances`.
 */
void dance_on_each_tap(tap_dance_state_t *state, void *user_data);

/**
 * Resolves a dance's taps to one outcome and holds that outcome's key; records the
 * held key for dance_reset().
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data The dance's definition in `dances`.
 */
void dance_finished(tap_dance_state_t *state, void *user_data);

/**
 * Waits 10 ms, releases the key dance_finished() held, if any, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance; unused.
 * @param user_data The dance's definition in `dances`.
 */
void dance_reset(tap_dance_state_t *state, void *user_data);
