/** @file
 * Layers and combos of the qgmlwy keymap, and its key event, housekeeping, start-up
 * and key light hooks.
 *
 * QMK compiles this file only inside its keymap introspection, which sizes
 * `keymaps` and `key_combos` with `sizeof`. Thus the two tables are in this file.
 * Each feature is in its own module, which `rules.mk` compiles.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "combos/combos.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "layer_lights/layer_lights.h"
#include "layers/layers.h"
#include "plain_mode/plain_mode.h"
#include "precision_key/precision_key.h"
#include "profiling/profiling.h"
#include "special_mode/special_mode.h"
#include "status_leds/status_leds.h"
#include "symbol_holds/symbol_holds.h"

// #endregion

// #region Types

/**
 * Key event handler: false when it fully handled the event, so no later handler and
 * no QMK code gets the event.
 */
typedef bool (*record_handler_t)(uint16_t keycode, keyrecord_t *record);

/** Key event handler, and if it also operates in the plain mode */
typedef struct {
    record_handler_t handler;    /**< Handler of the key events */
    bool operates_in_plain_mode; /**< False if it gets no push in the plain mode */
} record_step_t;

/** Handler for each pass of QMK's main loop */
typedef void (*housekeeping_handler_t)(void);

// #endregion

// #region Data

/** Keys of each layer, indexed by layer, matrix row and matrix column */
const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    {
        [LAYER_BASE][LEFT_NUMBER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_BASE][LEFT_TOP_ROW] =
            {
                KEY_HOLD(KC_TAB),
                KEY_HOLD(KC_Q),
                KEY_HOLD(KC_G),
                KEY_HOLD(KC_M),
                KEY_HOLD(KC_L),
                KEY_HOLD(KC_W),
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
                PROFILING_TOGGLE_KEY,
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
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_BASE][RIGHT_TOP_ROW] =
            {
                CAMEL_CASE_KEY,
                KEY_HOLD(KC_Y),
                KEY_HOLD(KC_F),
                KEY_HOLD(KC_U),
                KEY_HOLD(KC_B),
                KEY_HOLD(KC_SEMICOLON),
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
                KC_NO,
            },
        [LAYER_BASE][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_BASE][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = TG(LAYER_NAV),
                [RIGHT_OUTER_THUMB]  = KC_NO,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
            },

        [LAYER_NAV][LEFT_NUMBER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][LEFT_TOP_ROW] =
            {
                KC_NO,
                KC_NO,
                QK_MOUSE_CURSOR_UP,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][LEFT_HOME_ROW] =
            {
                KC_NO,
                QK_MOUSE_CURSOR_LEFT,
                QK_MOUSE_CURSOR_DOWN,
                QK_MOUSE_CURSOR_RIGHT,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
            },
        [LAYER_NAV][LEFT_BOTTOM_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
        [LAYER_NAV][LEFT_THUMB_KEYS] =
            {
                [LEFT_LARGE_THUMB]  = KC_NO,
                [LEFT_OUTER_THUMB]  = KC_NO,
                [LEFT_MIDDLE_THUMB] = KC_NO,
                [LEFT_INNER_THUMB]  = KC_NO,
            },
        [LAYER_NAV][RIGHT_NUMBER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][RIGHT_TOP_ROW] =
            {
                KC_NO,
                QK_MOUSE_WHEEL_UP,
                QK_MOUSE_BUTTON_1,
                KC_UP,
                QK_MOUSE_BUTTON_2,
                KC_HOME,
                KC_NO,
            },
        [LAYER_NAV][RIGHT_HOME_ROW] =
            {
                KC_NO,
                QK_MOUSE_WHEEL_DOWN,
                KC_LEFT,
                KC_DOWN,
                KC_RIGHT,
                KC_END,
                KC_NO,
            },
        [LAYER_NAV][RIGHT_BOTTOM_LETTER_ROW] =
            {
                NO_PHYSICAL_KEY,
                KC_NO,
                QK_MOUSE_BUTTON_4,
                QK_MOUSE_BUTTON_3,
                QK_MOUSE_BUTTON_5,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_NAV][RIGHT_THUMB_KEYS] =
            {
                [RIGHT_LARGE_THUMB]  = TG(LAYER_NAV),
                [RIGHT_OUTER_THUMB]  = QK_MOUSE_ACCELERATION_0,
                [RIGHT_MIDDLE_THUMB] = KC_NO,
                [RIGHT_INNER_THUMB]  = KC_NO,
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
                PLAIN_MODE_COMBO_LEFT_POSITION,
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
                PLAIN_MODE_COMBO_RIGHT_POSITION,
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
                KC_GRAVE,
                KC_AMPERSAND,
                KC_ASTERISK,
                KC_LEFT_PAREN,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][LEFT_HOME_ROW] =
            {
                SPECIAL_MODE_KEY,
                KC_TILDE,
                KC_DOLLAR,
                KC_PERCENT,
                KC_CIRCUMFLEX,
                KC_NO,
                KC_NO,
            },
        [LAYER_SPECIAL][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_EXCLAIM,
                KC_AT,
                KC_HASH,
                KC_NO,
                NO_PHYSICAL_KEY,
            },
        [LAYER_SPECIAL][LEFT_BOTTOM_ROW] =
            {
                KC_NO,
                KC_NO,
                KC_RIGHT_PAREN,
                KC_PLUS,
                KC_EQUAL,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
        [LAYER_SPECIAL][LEFT_THUMB_KEYS] =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_DELETE,
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

        [LAYER_PLAIN][LEFT_NUMBER_ROW] =
            {
                KC_ESCAPE,
                KC_GRAVE,
                KC_1,
                KC_2,
                KC_3,
                KC_4,
                KC_5,
            },
        [LAYER_PLAIN][LEFT_TOP_ROW] =
            {
                KC_TAB,
                KC_Q,
                KC_W,
                KC_E,
                KC_R,
                KC_T,
                KC_NO,
            },
        [LAYER_PLAIN][LEFT_HOME_ROW] =
            {
                KC_NO,
                KC_A,
                KC_S,
                KC_D,
                KC_F,
                KC_G,
                KC_NO,
            },
        [LAYER_PLAIN][LEFT_BOTTOM_LETTER_ROW] =
            {
                KC_LEFT_SHIFT,
                KC_Z,
                KC_X,
                KC_C,
                KC_V,
                KC_B,
                NO_PHYSICAL_KEY,
            },
        [LAYER_PLAIN][LEFT_BOTTOM_ROW] =
            {
                KC_LEFT_CTRL,
                KC_LEFT_ALT,
                KC_LEFT_GUI,
                KC_NO,
                KC_NO,
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
            },
        [LAYER_PLAIN][LEFT_THUMB_KEYS] =
            {
                [LEFT_LARGE_THUMB]  = KC_ENTER,
                [LEFT_OUTER_THUMB]  = KC_SPACE,
                [LEFT_MIDDLE_THUMB] = KC_BACKSPACE,
                [LEFT_INNER_THUMB]  = KC_DELETE,
            },
        [LAYER_PLAIN][RIGHT_NUMBER_ROW] =
            {
                KC_6,
                KC_7,
                KC_8,
                KC_9,
                KC_0,
                KC_MINUS,
                KC_EQUAL,
            },
        [LAYER_PLAIN][RIGHT_TOP_ROW] =
            {
                KC_NO,
                KC_Y,
                KC_U,
                KC_I,
                KC_O,
                KC_P,
                KC_BACKSLASH,
            },
        [LAYER_PLAIN][RIGHT_HOME_ROW] =
            {
                KC_NO,
                KC_H,
                KC_J,
                KC_K,
                KC_L,
                KC_SEMICOLON,
                KC_QUOTE,
            },
        [LAYER_PLAIN][RIGHT_BOTTOM_LETTER_ROW] =
            {
                NO_PHYSICAL_KEY,
                KC_N,
                KC_M,
                KC_COMMA,
                KC_DOT,
                KC_SLASH,
                KC_NO,
            },
        [LAYER_PLAIN][RIGHT_BOTTOM_ROW] =
            {
                NO_PHYSICAL_KEY,
                NO_PHYSICAL_KEY,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
                KC_NO,
            },
        [LAYER_PLAIN][RIGHT_THUMB_KEYS] =
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

/** Keys of the plain-mode combo, from LAYER_COMBO_POSITIONS */
static const uint16_t PROGMEM plain_mode_combo_keys[] = {
    PLAIN_MODE_COMBO_LEFT_POSITION,
    PLAIN_MODE_COMBO_RIGHT_POSITION,
    COMBO_END,
};

/** QMK combos, indexed by `combo`; each combo sends a keymap keycode */
combo_t key_combos[] = {
    [LOGGING_COMBO]    = COMBO(logging_combo_keys, LOGGING_TOGGLE),
    [PLAIN_MODE_COMBO] = COMBO(plain_mode_combo_keys, PLAIN_MODE_TOGGLE),
};

/**
 * Key event handlers, in call order. Log is first, so it gets each event. Plain
 * mode stops all handlers of the normal mode, so the keys act as on a standard
 * keyboard.
 */
static const record_step_t record_handlers[] = {
    {.handler = process_key_event_log, .operates_in_plain_mode = true},
    {.handler = process_plain_mode, .operates_in_plain_mode = true},
    {.handler = process_case_modes, .operates_in_plain_mode = false},
    {.handler = process_bracket_pairs, .operates_in_plain_mode = false},
    {.handler = process_special_mode, .operates_in_plain_mode = false},
    {.handler = process_symbol_holds, .operates_in_plain_mode = false},
    {.handler = process_precision_key, .operates_in_plain_mode = false},
};

// Each record handler needs the name of its profile record
_Static_assert(ARRAY_SIZE(record_handlers) == PROFILED_HANDLER_COUNT, "Profile names");

/** Housekeeping handlers, in call order; the LEDs come last, so they show the result */
static const housekeeping_handler_t housekeeping_handlers[] = {
    key_event_log_housekeeping,
    update_combos,
    update_status_leds,
};

// #endregion

// #region Static functions

/**
 * Tells if the plain mode stops a record handler for a key event. Plain mode stops
 * each push to a handler of the normal mode, but no release. Thus a key that the
 * operator pushed before the plain mode started does not stay pushed.
 *
 * @param step   Record handler.
 * @param record Key event.
 * @return True if the handler must not get the event.
 */
static bool plain_mode_stops(const record_step_t *step, const keyrecord_t *record) {
    if (step->operates_in_plain_mode || !record->event.pressed) {
        return false;
    }
    return plain_mode_is_on();
}

/**
 * Gives each key event to the record handlers in call order, until one handler
 * fully handles it.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when a handler fully handled the event, true otherwise.
 */
static bool dispatch_key_event(uint16_t keycode, keyrecord_t *record) {
    // QMK clears weak modifiers when a push arrives, but a push that waits in the
    // tap-hold buffer comes here after a symbol hold added its weak Shift
    if (record->event.pressed) {
        clear_weak_mods();
    }
    for (size_t index = 0; index < ARRAY_SIZE(record_handlers); index++) {
        const record_step_t *step = &record_handlers[index];
        if (plain_mode_stops(step, record)) {
            continue;
        }
        if (!PROFILED_RECORD_HANDLER(index, step->handler, keycode, record)) {
            return false;
        }
    }
    return true;
}

// #endregion

// #region Public functions

/**
 * Gives each key event to the dispatcher. The profiling debug build also measures
 * the call, and handles its toggle key.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when the keymap fully handled the event, true otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return PROFILED_RECORD_USER(dispatch_key_event, keycode, record);
}

/** Runs each housekeeping handler, on each pass of QMK's main loop */
void housekeeping_task_user(void) {
    for (size_t index = 0; index < ARRAY_SIZE(housekeeping_handlers); index++) {
        housekeeping_handlers[index]();
    }
}

/** Starts the key lights, after the keyboard code turned on the RGB matrix */
void keyboard_post_init_user(void) {
    start_layer_lights();
}

/**
 * Sets the mode color on all keys, after QMK made each frame of the RGB matrix effect.
 *
 * @return False, so that keyboard code does not set its own key colors after this hook.
 */
bool rgb_matrix_indicators_user(void) {
    show_layer_light();
    return false;
}

// #endregion
