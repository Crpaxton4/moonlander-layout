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

dance_t dances[DANCE_COUNT] = {
    [DANCE_CTRL_OR_GUI] =
        {
            .tap                   = KC_LEFT_CTRL,
            .hold                  = KC_LEFT_CTRL,
            .double_tap            = KC_LEFT_GUI,
            .double_hold           = KC_LEFT_GUI,
            .repeats_on_triple_tap = true,
        },
    [DANCE_LEFT_BRACKETS] =
        {
            .tap                   = KC_LEFT_PAREN,
            .hold                  = KC_LEFT_CURLY_BRACE,
            .double_tap            = KC_LEFT_BRACKET,
            .repeats_on_triple_tap = true,
        },
    [DANCE_RIGHT_BRACKETS] =
        {
            .tap                   = KC_RIGHT_PAREN,
            .hold                  = KC_RIGHT_CURLY_BRACE,
            .double_tap            = KC_RIGHT_BRACKET,
            .repeats_on_triple_tap = true,
        },
    [DANCE_MIDDLE_CLICK_OR_WHEEL] =
        {
            .tap                        = QK_MOUSE_BUTTON_3,
            .hold                       = QK_MOUSE_WHEEL_DOWN,
            .double_tap                 = QK_MOUSE_BUTTON_3,
            .double_hold                = QK_MOUSE_WHEEL_LEFT,
            .registers_double_tap_twice = true,
            .repeats_on_triple_tap      = true,
        },
    [DANCE_RIGHT_CLICK_OR_WHEEL] =
        {
            .tap                        = QK_MOUSE_BUTTON_2,
            .hold                       = QK_MOUSE_WHEEL_UP,
            .double_tap                 = QK_MOUSE_BUTTON_2,
            .double_hold                = QK_MOUSE_WHEEL_RIGHT,
            .registers_double_tap_twice = true,
            .repeats_on_triple_tap      = true,
        },
};
