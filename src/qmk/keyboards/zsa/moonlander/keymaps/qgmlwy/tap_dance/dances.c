/** @file
 * What each tap dance of the qgmlwy keymap sends, one definition per slot.
 *
 * Each row names the dance's outcomes. A keycode row leaves out the outcomes that
 * do nothing; a layer row names all four, NO_LAYER for none. Keycode dances hold
 * their key until the dance resets, and two presses interrupted by another key tap
 * the tap key, then hold it. Rows with `repeats_on_triple_tap` tap the tap key
 * three times on a third tap and once on each further tap. Layer dances move to
 * their layer, which stays after the reset; two interrupted presses move to the
 * tap layer.
 */

#include QMK_KEYBOARD_H

#include "layers/layers.h"
#include "tap_dance/tap_dance.h"

const dance_t dances[DANCE_COUNT] = {
    /** Tap or hold: Left Ctrl; double tap or double hold: Left GUI. */
    [DANCE_0] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = KC_LEFT_CTRL,
            .keycodes.hold                  = KC_LEFT_CTRL,
            .keycodes.double_tap            = KC_LEFT_GUI,
            .keycodes.double_hold           = KC_LEFT_GUI,
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: navigation gate layer; double tap: system layer. */
    [DANCE_1] =
        {
            .kind               = DANCE_LAYERS,
            .layers.tap         = LAYER_NAVIGATION_GATE,
            .layers.hold        = NO_LAYER,
            .layers.double_tap  = LAYER_SYSTEM,
            .layers.double_hold = NO_LAYER,
        },
    /** Tap: left paren; hold: left brace; double tap: left bracket. */
    [DANCE_2] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = KC_LPRN,
            .keycodes.hold                  = KC_LCBR,
            .keycodes.double_tap            = KC_LBRC,
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: right paren; hold: right brace; double tap: right bracket. */
    [DANCE_3] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = KC_RPRN,
            .keycodes.hold                  = KC_RCBR,
            .keycodes.double_tap            = KC_RBRC,
            .keycodes.repeats_on_triple_tap = true,
        },
    /**
     * Tap: middle click; hold: wheel down; double tap: middle click, registered
     * twice; double hold: wheel left.
     */
    [DANCE_4] =
        {
            .kind                                = DANCE_KEYCODES,
            .keycodes.tap                        = KC_MS_BTN3,
            .keycodes.hold                       = KC_MS_WH_DOWN,
            .keycodes.double_tap                 = KC_MS_BTN3,
            .keycodes.double_hold                = KC_MS_WH_LEFT,
            .keycodes.registers_double_tap_twice = true,
            .keycodes.repeats_on_triple_tap      = true,
        },
    /**
     * Tap: right click; hold: wheel up; double tap: right click, registered twice;
     * double hold: wheel right.
     */
    [DANCE_5] =
        {
            .kind                                = DANCE_KEYCODES,
            .keycodes.tap                        = KC_MS_BTN2,
            .keycodes.hold                       = KC_MS_WH_UP,
            .keycodes.double_tap                 = KC_MS_BTN2,
            .keycodes.double_hold                = KC_MS_WH_RIGHT,
            .keycodes.registers_double_tap_twice = true,
            .keycodes.repeats_on_triple_tap      = true,
        },
    /**
     * Tap: nothing; hold: Ctrl+Alt+Delete; double tap: Ctrl+Alt+Shift+Delete;
     * double hold: System Power.
     */
    [DANCE_6] =
        {
            .kind                 = DANCE_KEYCODES,
            .keycodes.hold        = LALT(LCTL(KC_DELETE)),
            .keycodes.double_tap  = LALT(LCTL(LSFT(KC_DELETE))),
            .keycodes.double_hold = KC_SYSTEM_POWER,
        },
    /** Tap: Ctrl+D; double tap: Ctrl+Shift+D. */
    [DANCE_7] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(KC_D),
            .keycodes.double_tap            = LCTL(LSFT(KC_D)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+Shift+J; double tap: Ctrl+Shift+I. */
    [DANCE_8] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(LSFT(KC_J)),
            .keycodes.double_tap            = LCTL(LSFT(KC_I)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+]; double tap: Ctrl+[. */
    [DANCE_9] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(KC_RBRC),
            .keycodes.double_tap            = LCTL(KC_LBRC),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+R; double tap: Ctrl+Shift+R. */
    [DANCE_10] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(KC_R),
            .keycodes.double_tap            = LCTL(LSFT(KC_R)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+Shift+Tab; double tap: Ctrl+1; double hold: Ctrl+Shift+Page Up. */
    [DANCE_11] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(LSFT(KC_TAB)),
            .keycodes.double_tap            = LCTL(KC_1),
            .keycodes.double_hold           = LCTL(LSFT(KC_PAGE_UP)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+T; double tap: Ctrl+N; double hold: Ctrl+Shift+N. */
    [DANCE_12] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(KC_T),
            .keycodes.double_tap            = LCTL(KC_N),
            .keycodes.double_hold           = LCTL(LSFT(KC_N)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: Ctrl+Tab; double tap: Ctrl+9; double hold: Ctrl+Shift+Page Down. */
    [DANCE_13] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LCTL(KC_TAB),
            .keycodes.double_tap            = LCTL(KC_9),
            .keycodes.double_hold           = LCTL(LSFT(KC_PGDN)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Tap: GUI+F5; double tap: GUI+Shift+F5. */
    [DANCE_14] =
        {
            .kind                           = DANCE_KEYCODES,
            .keycodes.tap                   = LGUI(KC_F5),
            .keycodes.double_tap            = LGUI(LSFT(KC_F5)),
            .keycodes.repeats_on_triple_tap = true,
        },
    /** Double hold: base layer, the way out of the lock layer. */
    [DANCE_15] =
        {
            .kind               = DANCE_LAYERS,
            .layers.tap         = NO_LAYER,
            .layers.hold        = NO_LAYER,
            .layers.double_tap  = NO_LAYER,
            .layers.double_hold = LAYER_BASE,
        },
    /** Double hold: base layer, the way out of the lock layer. */
    [DANCE_16] =
        {
            .kind               = DANCE_LAYERS,
            .layers.tap         = NO_LAYER,
            .layers.hold        = NO_LAYER,
            .layers.double_tap  = NO_LAYER,
            .layers.double_hold = LAYER_BASE,
        },
};
