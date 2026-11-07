/** @file
 * Layers and combos of the qgmlwy keymap, and its key event and housekeeping
 * hooks.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps` and `key_combos` with `sizeof`. Thus the two tables are in this file.
 * Each feature is in its own module, which `rules.mk` compiles.
 */

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "combos/combos.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "precision_key/precision_key.h"
#include "special_mode/special_mode.h"
#include "status_leds/status_leds.h"
#include "symbol_holds/symbol_holds.h"

/** Keys of each layer, indexed by layer, matrix row and matrix column */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    {
        [LAYER_BASE][LEFT_NUMBER_ROW] =
            {
                KC_ESCAPE,
                KC_F1,
                KC_F2,
                KC_F3,
                KC_F4,
                KC_F5,
                KC_F6,
            },
        [LAYER_BASE][LEFT_TOP_ROW] =
            {
                KC_TAB,
                KC_Q,
                KC_G,
                KC_M,
                KC_L,
                KC_W,
                SNAKE_CASE_KEY,
            },
        [LAYER_BASE][LEFT_HOME_ROW] =
            {
                SPECIAL_MODE_KEY,
                LCTL_T(KC_D),
                LALT_T(KC_S),
                LGUI_T(KC_T),
                LSFT_T(KC_N),
                KC_R,
                CONSTANT_CASE_KEY,
            },
        [LAYER_BASE][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_NO,
                SYMBOL_HOLD(KC_Z),
                SYMBOL_HOLD(KC_X),
                SYMBOL_HOLD(KC_C),
                SYMBOL_HOLD(KC_V),
                SYMBOL_HOLD(KC_J),
                NO_PHYSICAL_KEY,
            },
        [LAYER_BASE][LEFT_BOTTOM_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                QK_MOUSE_ACCELERATION_0,
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
                KC_F7,
                KC_F8,
                KC_F9,
                KC_F10,
                KC_F11,
                KC_F12,
                KC_NO,
            },
        [LAYER_BASE][RIGHT_TOP_ROW] =
            {
                CAMEL_CASE_KEY,
                KC_Y,
                KC_F,
                KC_U,
                KC_B,
                KC_SEMICOLON,
                KC_BACKSLASH,
            },
        [LAYER_BASE][RIGHT_HOME_ROW] =
            {
                PASCAL_CASE_KEY,
                KC_I,
                RSFT_T(KC_A),
                RGUI_T(KC_E),
                LALT_T(KC_O),
                RCTL_T(KC_H),
                KC_QUOTE,
            },
        [LAYER_BASE][RIGHT_BOTTOM_LETTER_ROW] =
            {
                NO_PHYSICAL_KEY,
                SYMBOL_HOLD(KC_K),
                SYMBOL_HOLD(KC_P),
                SYMBOL_HOLD(KC_COMMA),
                SYMBOL_HOLD(KC_DOT),
                SYMBOL_HOLD(KC_SLASH),
                QK_MOUSE_WHEEL_UP,
            },
        [LAYER_BASE][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                QK_MOUSE_CURSOR_LEFT,
                QK_MOUSE_CURSOR_UP,
                QK_MOUSE_CURSOR_DOWN,
                QK_MOUSE_CURSOR_RIGHT,
                QK_MOUSE_WHEEL_DOWN,
            },
        [LAYER_BASE][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = TG(LAYER_ARROWS),
                [RIGHT_OUTER_THUMB]  = QK_MOUSE_BUTTON_1,
                [RIGHT_MIDDLE_THUMB] = QK_MOUSE_BUTTON_2,
                [RIGHT_INNER_THUMB]  = QK_MOUSE_BUTTON_3,
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
                KC_HOME,
            },
        [LAYER_ARROWS][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_LEFT,
                KC_UP,
                KC_DOWN,
                KC_RIGHT,
                KC_END,
            },
        [LAYER_ARROWS][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = KC_TRANSPARENT,
                [RIGHT_OUTER_THUMB]  = KC_TRANSPARENT,
                [RIGHT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [RIGHT_INNER_THUMB]  = KC_TRANSPARENT,
            },

        [LAYER_COMBO_POSITIONS][LEFT_NUMBER_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][LEFT_TOP_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                LOGGING_COMBO_LEFT_POSITION,
            },
        [LAYER_COMBO_POSITIONS][LEFT_HOME_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                NO_PHYSICAL_KEY,
            },
        [LAYER_COMBO_POSITIONS][LEFT_BOTTOM_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
        [LAYER_COMBO_POSITIONS][LEFT_THUMB_KEYS] =
            {
                [LEFT_LARGE_THUMB]  = KC_TRANSPARENT,
                [LEFT_OUTER_THUMB]  = KC_TRANSPARENT,
                [LEFT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [LEFT_INNER_THUMB]  = KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_NUMBER_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_TOP_ROW] =
            {
                LOGGING_COMBO_RIGHT_POSITION,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_HOME_ROW] =
            {
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_BOTTOM_LETTER_ROW] =
            {
                NO_PHYSICAL_KEY,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
                KC_TRANSPARENT,
            },
        [LAYER_COMBO_POSITIONS][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = KC_TRANSPARENT,
                [RIGHT_OUTER_THUMB]  = KC_TRANSPARENT,
                [RIGHT_MIDDLE_THUMB] = KC_TRANSPARENT,
                [RIGHT_INNER_THUMB]  = KC_TRANSPARENT,
            },

        [LAYER_SPECIAL][LEFT_NUMBER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][LEFT_TOP_ROW] =
            {
                KC_TAB,
                KC_TILDE,
                KC_AT,
                KC_CIRCUMFLEX,
                KC_PERCENT,
                KC_GRAVE,
                KC_NO,
            },
        [LAYER_SPECIAL][LEFT_HOME_ROW] =
            {
                SPECIAL_MODE_KEY,
                KC_EXCLAIM,
                KC_ASTERISK,
                KC_DOLLAR,
                KC_EQUAL,
                KC_AMPERSAND,
                KC_NO,
            },
        [LAYER_SPECIAL][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_NO,
                KC_HASH,
                KC_PIPE,
                KC_PLUS,
                KC_MINUS,
                KC_NO,
                NO_PHYSICAL_KEY,
            },
        [LAYER_SPECIAL][LEFT_BOTTOM_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
        [LAYER_SPECIAL][LEFT_THUMB_KEYS] =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_NUMBER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_TOP_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_7,
                KC_8,
                KC_9,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_HOME_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_4,
                KC_5,
                KC_6,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_BOTTOM_LETTER_ROW] =
            {
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_1,
                KC_2,
                KC_3,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_0,
                KC_DOT,
                KC_COMMA,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = KC_NO,
                [RIGHT_OUTER_THUMB]  = KC_NO,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
            },
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

/** Key event handlers, in call order; the log is first, so it gets each event */
static const record_handler_t record_handlers[] = {
    process_key_event_log, process_case_modes,   process_bracket_pairs,
    process_special_mode,  process_symbol_holds, process_precision_key,
};

/** Handler for each pass of QMK's main loop */
typedef void (*housekeeping_handler_t)(void);

/** Housekeeping handlers, in call order; the LEDs come last, so they show the result */
static const housekeeping_handler_t housekeeping_handlers[] = {
    key_event_log_housekeeping,
    update_status_leds,
};

/**
 * Gives each key event to each record handler, until one handler fully handles it.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when a handler fully handled the event, true otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // QMK clears weak modifiers when a push arrives, but a push that waits in the
    // tap-hold buffer comes here after a symbol hold added its weak Shift
    if (record->event.pressed) {
        clear_weak_mods();
    }
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
