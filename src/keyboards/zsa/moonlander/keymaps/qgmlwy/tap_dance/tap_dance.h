/** @file
 * Tap dance slots of the qgmlwy keymap and their QMK callbacks.
 *
 * Each dance's finished callback resolves the taps to one outcome with
 * dance_step() and registers that outcome's key or moves to its layer; the
 * reset callback waits 10 ms and releases it. Two taps interrupted by another
 * key tap a keycode dance's single-tap key, then hold it until the reset. The
 * keymap's `tap_dance_actions` binds the callbacks to the slots.
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Tap dance slots, each the index of its action in `tap_dance_actions` and of
 * its record in `dance_state`.
 */
enum tap_dance_codes {
    DANCE_0,
    DANCE_1,
    DANCE_2,
    DANCE_3,
    DANCE_4,
    DANCE_5,
    DANCE_6,
    DANCE_7,
    DANCE_8,
    DANCE_9,
    DANCE_10,
    DANCE_11,
    DANCE_12,
    DANCE_13,
    DANCE_14,
    DANCE_15,
    DANCE_16,
    /** Number of tap dance slots; sizes `dance_state`. */
    DANCE_COUNT,
};

/**
 * Repeats Left Ctrl, the single-tap key of dance 0, on long tap runs: three times at
 * the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_0(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 0 and sends its outcome: Left Ctrl on a tap or hold, Left GUI on a
 * double tap or double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_0_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_0_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_0_reset(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 1 and sends its outcome: moves to the navigation gate layer on a tap
 * or two interrupted taps and to the system layer on a double tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_1_finished(tap_dance_state_t *state, void *user_data);

/**
 * Ends dance 1 after a 10 ms wait; its layer moves need no release.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_1_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats `(`, the single-tap key of dance 2, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_2(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 2 and sends its outcome: `(` on a tap, `{` on a hold, `[` on a double
 * tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_2_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_2_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_2_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats `)`, the single-tap key of dance 3, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_3(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 3 and sends its outcome: `)` on a tap, `}` on a hold, `]` on a double
 * tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_3_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_3_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_3_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats the middle mouse button, the single-tap key of dance 4, on long tap runs:
 * three times at the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_4(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 4 and sends its outcome: the middle mouse button on a tap, wheel down
 * on a hold, the middle button registered twice on a double tap, wheel left on a
 * double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_4_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_4_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_4_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats the right mouse button, the single-tap key of dance 5, on long tap runs:
 * three times at the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_5(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 5 and sends its outcome: the right mouse button on a tap, wheel up on
 * a hold, the right button registered twice on a double tap, wheel right on a double
 * hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_5_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_5_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_5_reset(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 6 and sends its outcome: Ctrl+Alt+Delete on a hold,
 * Ctrl+Alt+Shift+Delete on a double tap, system power on a double hold; a tap does
 * nothing.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_6_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_6_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_6_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+D, the single-tap key of dance 7, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_7(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 7 and sends its outcome: Ctrl+D on a tap, Ctrl+Shift+D on a double
 * tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_7_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_7_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_7_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+Shift+J, the single-tap key of dance 8, on long tap runs: three times
 * at the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_8(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 8 and sends its outcome: Ctrl+Shift+J on a tap, Ctrl+Shift+I on a
 * double tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_8_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_8_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_8_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+], the single-tap key of dance 9, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_9(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 9 and sends its outcome: Ctrl+] on a tap, Ctrl+[ on a double tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_9_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_9_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_9_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+R, the single-tap key of dance 10, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_10(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 10 and sends its outcome: Ctrl+R on a tap, Ctrl+Shift+R on a double
 * tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_10_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_10_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_10_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+Shift+Tab, the single-tap key of dance 11, on long tap runs: three
 * times at the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_11(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 11 and sends its outcome: Ctrl+Shift+Tab on a tap, Ctrl+1 on a double
 * tap, Ctrl+Shift+Page Up on a double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_11_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_11_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_11_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+T, the single-tap key of dance 12, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_12(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 12 and sends its outcome: Ctrl+T on a tap, Ctrl+N on a double tap,
 * Ctrl+Shift+N on a double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_12_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_12_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_12_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats Ctrl+Tab, the single-tap key of dance 13, on long tap runs: three times at
 * the third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_13(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 13 and sends its outcome: Ctrl+Tab on a tap, Ctrl+9 on a double tap,
 * Ctrl+Shift+Page Down on a double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_13_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_13_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_13_reset(tap_dance_state_t *state, void *user_data);

/**
 * Repeats GUI+F5, the single-tap key of dance 14, on long tap runs: three times at the
 * third tap, then once per further tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void on_dance_14(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 14 and sends its outcome: GUI+F5 on a tap, GUI+Shift+F5 on a double
 * tap.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_14_finished(tap_dance_state_t *state, void *user_data);

/**
 * Releases what dance_14_finished() registered, after a 10 ms wait, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_14_reset(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 15 and sends its outcome: moves to the base layer on a double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_15_finished(tap_dance_state_t *state, void *user_data);

/**
 * Ends dance 15 after a 10 ms wait; its layer moves need no release.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_15_reset(tap_dance_state_t *state, void *user_data);

/**
 * Resolves dance 16 and sends its outcome: moves to the base layer on a double hold.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_16_finished(tap_dance_state_t *state, void *user_data);

/**
 * Ends dance 16 after a 10 ms wait; its layer moves need no release.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data Action data from `tap_dance_actions`; unused.
 */
void dance_16_reset(tap_dance_state_t *state, void *user_data);
