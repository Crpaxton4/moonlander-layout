/** @file
 * Layers and tap dance actions of the qgmlwy keymap, and its key event hook.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps` and `tap_dance_actions` with `sizeof`, so both tables are defined
 * here. Each feature lives in its own module, compiled by `rules.mk`.
 */

#include QMK_KEYBOARD_H

#include "dual_function/dual_function.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "tap_dance/tap_dance.h"

/// Keys of each layer, indexed by layer, matrix row and matrix column
const uint16_t PROGMEM
    keymaps[][MATRIX_ROWS][MATRIX_COLS] =
        {
            [LAYER_BASE][LEFT_NUMBER_ROW] =
                {
                    KC_ESCAPE,
                    KC_GRAVE,
                    KC_1,
                    KC_2,
                    KC_3,
                    KC_4,
                    KC_5,
                },
            [LAYER_BASE][LEFT_TOP_ROW] =
                {
                    KC_TAB,
                    KC_Q,
                    KC_G,
                    KC_M,
                    KC_L,
                    KC_W,
                    COPY_KEY,
                },
            [LAYER_BASE][LEFT_HOME_ROW] =
                {
                    KC_NO,
                    KC_D,
                    KC_S,
                    KC_T,
                    KC_N,
                    KC_R,
                    PASTE_KEY,
                },
            [LAYER_BASE][LEFT_BOTTOM_LETTER_ROW] =
                {
                    KC_LEFT_SHIFT,
                    KC_Z,
                    KC_X,
                    KC_C,
                    KC_V,
                    KC_J,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_BASE][LEFT_BOTTOM_ROW] =
                {
                    TD(DANCE_CTRL_OR_GUI),
                    KC_LEFT_ALT,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_BASE][LEFT_THUMB_KEYS] =
                {
                    [LEFT_LARGE_THUMB]  = KC_ENTER,
                    [LEFT_OUTER_THUMB]  = KC_SPACE,
                    [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                    [LEFT_INNER_THUMB]  = KC_DELETE,
                },
            [LAYER_BASE][RIGHT_NUMBER_ROW] =
                {
                    KC_6,
                    KC_7,
                    KC_8,
                    KC_9,
                    KC_0,
                    KC_MINUS,
                    KC_EQUAL,
                },
            [LAYER_BASE][RIGHT_TOP_ROW] =
                {
                    KC_NO,
                    KC_Y,
                    KC_F,
                    KC_U,
                    KC_B,
                    KC_SEMICOLON,
                    KC_BACKSLASH,
                },
            [LAYER_BASE][RIGHT_HOME_ROW] =
                {
                    KC_NO,
                    KC_I,
                    KC_A,
                    KC_E,
                    KC_O,
                    KC_H,
                    KC_QUOTE,
                },
            [LAYER_BASE][RIGHT_BOTTOM_LETTER_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    KC_K,
                    KC_P,
                    KC_COMMA,
                    KC_DOT,
                    KC_SLASH,
                    TD(DANCE_LEFT_BRACKETS),
                },
            [LAYER_BASE][RIGHT_BOTTOM_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                    QK_MOUSE_CURSOR_LEFT,
                    QK_MOUSE_CURSOR_UP,
                    QK_MOUSE_CURSOR_DOWN,
                    QK_MOUSE_CURSOR_RIGHT,
                    TD(DANCE_RIGHT_BRACKETS),
                },
            [LAYER_BASE][RIGHT_THUMB_KEYS] =
                {
                    [RIGHT_LARGE_THUMB]  = TO(LAYER_ARROWS),
                    [RIGHT_OUTER_THUMB]  = QK_MOUSE_BUTTON_1,
                    [RIGHT_MIDDLE_THUMB] = TD(DANCE_RIGHT_CLICK_OR_WHEEL),
                    [RIGHT_INNER_THUMB]  = TD(DANCE_MIDDLE_CLICK_OR_WHEEL),
                },

            [LAYER_ARROWS][LEFT_NUMBER_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][LEFT_TOP_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][LEFT_HOME_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][LEFT_BOTTOM_LETTER_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_ARROWS][LEFT_BOTTOM_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_ARROWS][LEFT_THUMB_KEYS] =
                {
                    [LEFT_LARGE_THUMB]  = KC_TRANSPARENT,
                    [LEFT_OUTER_THUMB]  = KC_TRANSPARENT,
                    [LEFT_MIDDLE_THUMB] = KC_TRANSPARENT,
                    [LEFT_INNER_THUMB]  = KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_NUMBER_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_TOP_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_HOME_ROW] =
                {
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_BOTTOM_LETTER_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_BOTTOM_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                    KC_LEFT,
                    KC_UP,
                    KC_DOWN,
                    KC_RIGHT,
                    KC_TRANSPARENT,
                },
            [LAYER_ARROWS][RIGHT_THUMB_KEYS] =
                {
                    [RIGHT_LARGE_THUMB]  = TO(LAYER_BASE),
                    [RIGHT_OUTER_THUMB]  = KC_TRANSPARENT,
                    [RIGHT_MIDDLE_THUMB] = KC_TRANSPARENT,
                    [RIGHT_INNER_THUMB]  = KC_TRANSPARENT,
                },
};

/// QMK actions of the tap dances, indexed by slot; each plays its `dances` row
tap_dance_action_t tap_dance_actions[] = {
    [DANCE_CTRL_OR_GUI]           = ACTION_TAP_DANCE_TABLE(DANCE_CTRL_OR_GUI),
    [DANCE_LEFT_BRACKETS]         = ACTION_TAP_DANCE_TABLE(DANCE_LEFT_BRACKETS),
    [DANCE_RIGHT_BRACKETS]        = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_BRACKETS),
    [DANCE_MIDDLE_CLICK_OR_WHEEL] = ACTION_TAP_DANCE_TABLE(DANCE_MIDDLE_CLICK_OR_WHEEL),
    [DANCE_RIGHT_CLICK_OR_WHEEL]  = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_CLICK_OR_WHEEL),
};

/**
 * Hands each key event to the copy and paste keys.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when process_dual_function() fully handled the event, true
 *         otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return process_dual_function(keycode, record);
}
