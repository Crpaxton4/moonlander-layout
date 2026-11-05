/** @file
 * What each tap dance of the qgmlwy keymap sends, one definition per slot.
 *
 * Each row names the dance's outcomes; any outcome it leaves out does nothing.
 * Keycode dances hold their key until the dance resets, and two presses
 * interrupted by another key tap the tap key, then hold it. Rows with
 * `repeats_on_triple_tap` tap the tap key three times on a third tap and once on
 * each further tap. Layer dances move to their layer, which stays after the
 * reset; two interrupted presses move to the tap layer.
 */

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "tap_dance/tap_dance.h"

const dance_t dances[DANCE_COUNT] = {
    /** Tap or hold: Left Ctrl; double tap or double hold: Left GUI. */
    [DANCE_0] = KEYCODE_DANCE({
        .tap                   = KC_LEFT_CTRL,
        .hold                  = KC_LEFT_CTRL,
        .double_tap            = KC_LEFT_GUI,
        .double_hold           = KC_LEFT_GUI,
        .repeats_on_triple_tap = true,
    }),
    /** Tap: navigation gate layer; double tap: system layer. */
    [DANCE_1] = LAYER_DANCE(LAYER_NAVIGATION_GATE, NO_LAYER, LAYER_SYSTEM, NO_LAYER),
    /** Tap: left paren; hold: left brace; double tap: left bracket. */
    [DANCE_2] = KEYCODE_DANCE({
        .tap                   = KC_LPRN,
        .hold                  = KC_LCBR,
        .double_tap            = KC_LBRC,
        .repeats_on_triple_tap = true,
    }),
    /** Tap: right paren; hold: right brace; double tap: right bracket. */
    [DANCE_3] = KEYCODE_DANCE({
        .tap                   = KC_RPRN,
        .hold                  = KC_RCBR,
        .double_tap            = KC_RBRC,
        .repeats_on_triple_tap = true,
    }),
    /**
     * Tap: middle click; hold: wheel down; double tap: middle click, registered
     * twice; double hold: wheel left.
     */
    [DANCE_4] = KEYCODE_DANCE({
        .tap                        = KC_MS_BTN3,
        .hold                       = KC_MS_WH_DOWN,
        .double_tap                 = KC_MS_BTN3,
        .double_hold                = KC_MS_WH_LEFT,
        .registers_double_tap_twice = true,
        .repeats_on_triple_tap      = true,
    }),
    /**
     * Tap: right click; hold: wheel up; double tap: right click, registered twice;
     * double hold: wheel right.
     */
    [DANCE_5] = KEYCODE_DANCE({
        .tap                        = KC_MS_BTN2,
        .hold                       = KC_MS_WH_UP,
        .double_tap                 = KC_MS_BTN2,
        .double_hold                = KC_MS_WH_RIGHT,
        .registers_double_tap_twice = true,
        .repeats_on_triple_tap      = true,
    }),
    /**
     * Tap: nothing; hold: Ctrl+Alt+Delete; double tap: Ctrl+Alt+Shift+Delete;
     * double hold: System Power.
     */
    [DANCE_6] = KEYCODE_DANCE({
        .hold        = LALT(LCTL(KC_DELETE)),
        .double_tap  = LALT(LCTL(LSFT(KC_DELETE))),
        .double_hold = KC_SYSTEM_POWER,
    }),
    /** Tap: Ctrl+D; double tap: Ctrl+Shift+D. */
    [DANCE_7] = KEYCODE_DANCE({
        .tap                   = LCTL(KC_D),
        .double_tap            = LCTL(LSFT(KC_D)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+Shift+J; double tap: Ctrl+Shift+I. */
    [DANCE_8] = KEYCODE_DANCE({
        .tap                   = LCTL(LSFT(KC_J)),
        .double_tap            = LCTL(LSFT(KC_I)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+]; double tap: Ctrl+[. */
    [DANCE_9] = KEYCODE_DANCE({
        .tap                   = LCTL(KC_RBRC),
        .double_tap            = LCTL(KC_LBRC),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+R; double tap: Ctrl+Shift+R. */
    [DANCE_10] = KEYCODE_DANCE({
        .tap                   = LCTL(KC_R),
        .double_tap            = LCTL(LSFT(KC_R)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+Shift+Tab; double tap: Ctrl+1; double hold: Ctrl+Shift+Page Up. */
    [DANCE_11] = KEYCODE_DANCE({
        .tap                   = LCTL(LSFT(KC_TAB)),
        .double_tap            = LCTL(KC_1),
        .double_hold           = LCTL(LSFT(KC_PAGE_UP)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+T; double tap: Ctrl+N; double hold: Ctrl+Shift+N. */
    [DANCE_12] = KEYCODE_DANCE({
        .tap                   = LCTL(KC_T),
        .double_tap            = LCTL(KC_N),
        .double_hold           = LCTL(LSFT(KC_N)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: Ctrl+Tab; double tap: Ctrl+9; double hold: Ctrl+Shift+Page Down. */
    [DANCE_13] = KEYCODE_DANCE({
        .tap                   = LCTL(KC_TAB),
        .double_tap            = LCTL(KC_9),
        .double_hold           = LCTL(LSFT(KC_PGDN)),
        .repeats_on_triple_tap = true,
    }),
    /** Tap: GUI+F5; double tap: GUI+Shift+F5. */
    [DANCE_14] = KEYCODE_DANCE({
        .tap                   = LGUI(KC_F5),
        .double_tap            = LGUI(LSFT(KC_F5)),
        .repeats_on_triple_tap = true,
    }),
    /** Double hold: base layer, the way out of the lock layer. */
    [DANCE_15] = LAYER_DANCE(NO_LAYER, NO_LAYER, NO_LAYER, LAYER_BASE),
    /** Double hold: base layer, the way out of the lock layer. */
    [DANCE_16] = LAYER_DANCE(NO_LAYER, NO_LAYER, NO_LAYER, LAYER_BASE),
};
