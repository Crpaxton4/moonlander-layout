/** @file
 * What each tap dance of the qgmlwy keymap sends, one definition per slot.
 *
 * Each row names the key of each outcome and leaves out the outcomes that do
 * nothing. A key is held until the dance resets, and two presses interrupted by
 * another key tap the tap key, then hold it. Rows with `repeats_on_triple_tap` tap
 * the tap key three times on a third tap and once on each further tap.
 */

#include QMK_KEYBOARD_H

#include "tap_dance/tap_dance.h"

const dance_t dances[DANCE_COUNT] = {
    /** Tap or hold: Left Ctrl; double tap or double hold: Left GUI. */
    [DANCE_CTRL_OR_GUI] =
        {
            .tap                   = KC_LEFT_CTRL,
            .hold                  = KC_LEFT_CTRL,
            .double_tap            = KC_LEFT_GUI,
            .double_hold           = KC_LEFT_GUI,
            .repeats_on_triple_tap = true,
        },
    /** Tap: left paren; hold: left brace; double tap: left bracket. */
    [DANCE_LEFT_BRACKETS] =
        {
            .tap                   = KC_LPRN,
            .hold                  = KC_LCBR,
            .double_tap            = KC_LBRC,
            .repeats_on_triple_tap = true,
        },
    /** Tap: right paren; hold: right brace; double tap: right bracket. */
    [DANCE_RIGHT_BRACKETS] =
        {
            .tap                   = KC_RPRN,
            .hold                  = KC_RCBR,
            .double_tap            = KC_RBRC,
            .repeats_on_triple_tap = true,
        },
    /**
     * Tap: middle click; hold: wheel down; double tap: middle click, registered
     * twice; double hold: wheel left.
     */
    [DANCE_MIDDLE_CLICK_OR_WHEEL] =
        {
            .tap                        = KC_MS_BTN3,
            .hold                       = KC_MS_WH_DOWN,
            .double_tap                 = KC_MS_BTN3,
            .double_hold                = KC_MS_WH_LEFT,
            .registers_double_tap_twice = true,
            .repeats_on_triple_tap      = true,
        },
    /**
     * Tap: right click; hold: wheel up; double tap: right click, registered twice;
     * double hold: wheel right.
     */
    [DANCE_RIGHT_CLICK_OR_WHEEL] =
        {
            .tap                        = KC_MS_BTN2,
            .hold                       = KC_MS_WH_UP,
            .double_tap                 = KC_MS_BTN2,
            .double_hold                = KC_MS_WH_RIGHT,
            .registers_double_tap_twice = true,
            .repeats_on_triple_tap      = true,
        },
};
