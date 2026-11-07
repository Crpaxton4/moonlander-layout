/** @file
 * Layers, tap dance actions and combos of the qgmlwy keymap, and its key event and
 * housekeeping hooks.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps`, `tap_dance_actions` and `key_combos` with `sizeof`, so the three tables
 * are defined here. Each feature lives in its own module, compiled by `rules.mk`.
 */

#include QMK_KEYBOARD_H

#include "combos/combos.h"
#include "dual_function/dual_function.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "status_leds/status_leds.h"
#include "tap_dance/tap_dance.h"

/** Keys of each layer, indexed by layer, matrix row and matrix column */
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

            [LAYER_COMBO_POSITIONS][LEFT_NUMBER_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][LEFT_TOP_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    LOGGING_COMBO_LEFT_POSITION,
                },
            [LAYER_COMBO_POSITIONS][LEFT_HOME_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][LEFT_BOTTOM_LETTER_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_COMBO_POSITIONS][LEFT_BOTTOM_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                },
            [LAYER_COMBO_POSITIONS][LEFT_THUMB_KEYS] =
                {
                    [LEFT_LARGE_THUMB]  = KC_NO,
                    [LEFT_OUTER_THUMB]  = KC_NO,
                    [LEFT_MIDDLE_THUMB] = KC_NO,
                    [LEFT_INNER_THUMB]  = KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_NUMBER_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_TOP_ROW] =
                {
                    LOGGING_COMBO_RIGHT_POSITION,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_HOME_ROW] =
                {
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_BOTTOM_LETTER_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_BOTTOM_ROW] =
                {
                    NO_PHYSICAL_KEY,
                    NO_PHYSICAL_KEY,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                    KC_NO,
                },
            [LAYER_COMBO_POSITIONS][RIGHT_THUMB_KEYS] =
                {
                    [RIGHT_LARGE_THUMB]  = KC_NO,
                    [RIGHT_OUTER_THUMB]  = KC_NO,
                    [RIGHT_MIDDLE_THUMB] = KC_NO,
                    [RIGHT_INNER_THUMB]  = KC_NO,
                },
};

/** QMK actions of the tap dances, indexed by slot; each plays its `dances` row */
tap_dance_action_t tap_dance_actions[] = {
    [DANCE_CTRL_OR_GUI]           = ACTION_TAP_DANCE_TABLE(DANCE_CTRL_OR_GUI),
    [DANCE_LEFT_BRACKETS]         = ACTION_TAP_DANCE_TABLE(DANCE_LEFT_BRACKETS),
    [DANCE_RIGHT_BRACKETS]        = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_BRACKETS),
    [DANCE_MIDDLE_CLICK_OR_WHEEL] = ACTION_TAP_DANCE_TABLE(DANCE_MIDDLE_CLICK_OR_WHEEL),
    [DANCE_RIGHT_CLICK_OR_WHEEL]  = ACTION_TAP_DANCE_TABLE(DANCE_RIGHT_CLICK_OR_WHEEL),
};

/** Keys of the logging combo, from LAYER_COMBO_POSITIONS */
static const uint16_t PROGMEM logging_combo_keys[] = {
    LOGGING_COMBO_LEFT_POSITION,
    LOGGING_COMBO_RIGHT_POSITION,
    COMBO_END,
};

/** QMK combos, indexed by `combo`; the logging combo sends LOGGING_TOGGLE */
combo_t key_combos[] = {
    [LOGGING_COMBO] = COMBO(logging_combo_keys, LOGGING_TOGGLE),
};

/**
 * Key event handler: false when it fully handled the event, so no later handler and
 * no QMK code gets the event.
 */
typedef bool (*record_handler_t)(uint16_t keycode, keyrecord_t *record);

/** Key event handlers, in call order; the key event log is first, so it sees all */
static const record_handler_t record_handlers[] = {
    process_key_event_log,
    process_dual_function,
};

/** Handler for each pass of QMK's main loop */
typedef void (*housekeeping_handler_t)(void);

/** Housekeeping handlers, in call order; the LEDs come last, so they show the result */
static const housekeeping_handler_t housekeeping_handlers[] = {
    key_event_log_housekeeping,
    update_status_leds,
};

/**
 * Hands each key event to each record handler, until one handles it fully.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when a handler fully handled the event, true otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    for (size_t index = 0; index < ARRAY_SIZE(record_handlers); index++) {
        if (!record_handlers[index](keycode, record)) {
            return false;
        }
    }
    return true;
}

/** Runs each housekeeping handler, on each pass of QMK's main loop */
void housekeeping_task_user(void) {
    for (size_t index = 0; index < ARRAY_SIZE(housekeeping_handlers); index++) {
        housekeeping_handlers[index]();
    }
}
