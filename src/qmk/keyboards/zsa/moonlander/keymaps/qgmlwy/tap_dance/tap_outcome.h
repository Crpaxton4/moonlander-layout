/** @file
 * Outcomes of a tap dance's taps and its triple-tap repeat, worked out from plain
 * values so they need no QMK state.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/** Outcomes tap_outcome() resolves a tap dance to. */
enum dance_outcome {
    /** No outcome: the dance hasn't finished, or it was reset. */
    NO_OUTCOME = 0,
    /** Tapped once, or pressed once and interrupted by another key. */
    SINGLE_TAP,
    /** Pressed once and still held, uninterrupted. */
    SINGLE_HOLD,
    /** Tapped twice. */
    DOUBLE_TAP,
    /** Tapped once, then pressed again and still held. */
    DOUBLE_HOLD,
    /** Pressed twice and interrupted by another key. */
    DOUBLE_SINGLE_TAP,
    /** Any other tap count, none included. */
    MORE_TAPS,
};

/**
 * Resolves a tap dance's taps to one outcome.
 *
 * @param count       Number of times the dance key was pressed.
 * @param interrupted Whether another key was pressed during the dance.
 * @param pressed     Whether the dance key is still held down.
 * @return SINGLE_TAP to DOUBLE_SINGLE_TAP for one or two presses, MORE_TAPS for
 *         any other count.
 */
uint8_t tap_outcome(uint8_t count, bool interrupted, bool pressed);

/**
 * Number of times a repeating dance taps its key on reaching a tap count.
 *
 * The third tap sends the key once per tap so far, and each further tap sends it
 * once more; the first two taps send nothing.
 *
 * @param count Number of times the dance key has been pressed so far.
 * @return How many times to tap the dance's key now.
 */
uint8_t repeat_taps(uint8_t count);
