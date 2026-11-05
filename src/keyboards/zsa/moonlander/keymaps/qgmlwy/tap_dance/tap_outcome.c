/** @file
 * Tap dance outcomes and triple-tap repeat counts, from plain values only.
 */

#include "tap_dance/tap_outcome.h"

/** Tap count of a dance tapped or held once. */
#define SINGLE_TAP_COUNT 1

/** Tap count of a dance tapped twice, or tapped once and then held. */
#define DOUBLE_TAP_COUNT 2

/** Tap count at which a repeating dance starts repeating its key. */
#define REPEAT_TAP_COUNT 3

/** Taps a repeating dance sends for each tap after REPEAT_TAP_COUNT. */
#define TAPS_PER_FURTHER_TAP 1

/**
 * Resolves a dance pressed once.
 *
 * @param interrupted Whether another key was pressed during the dance.
 * @param pressed     Whether the dance key is still held down.
 * @return SINGLE_HOLD while held and uninterrupted, SINGLE_TAP otherwise.
 */
static uint8_t single_press_outcome(bool interrupted, bool pressed) {
    if (interrupted || !pressed) {
        return SINGLE_TAP;
    }
    return SINGLE_HOLD;
}

/**
 * Resolves a dance pressed twice.
 *
 * @param interrupted Whether another key was pressed during the dance.
 * @param pressed     Whether the dance key is still held down.
 * @return DOUBLE_SINGLE_TAP when interrupted, else DOUBLE_HOLD while held, else
 *         DOUBLE_TAP.
 */
static uint8_t double_press_outcome(bool interrupted, bool pressed) {
    if (interrupted) {
        return DOUBLE_SINGLE_TAP;
    }
    if (pressed) {
        return DOUBLE_HOLD;
    }
    return DOUBLE_TAP;
}

uint8_t tap_outcome(uint8_t count, bool interrupted, bool pressed) {
    if (count == SINGLE_TAP_COUNT) {
        return single_press_outcome(interrupted, pressed);
    }
    if (count == DOUBLE_TAP_COUNT) {
        return double_press_outcome(interrupted, pressed);
    }
    return MORE_TAPS;
}

uint8_t repeat_taps(uint8_t count) {
    if (count == REPEAT_TAP_COUNT) {
        return REPEAT_TAP_COUNT;
    }
    if (count > REPEAT_TAP_COUNT) {
        return TAPS_PER_FURTHER_TAP;
    }
    return 0;
}
