/** @file
 * Key sequences of the qgmlwy keymap's `ST_MACRO_*` keys: one table row per macro,
 * built from one template, and the lookup that sends a row.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "macros/macros.h"

/**
 * Milliseconds the window macros wait between their GUI and Ctrl chords.
 *
 * `SS_DELAY()` turns its argument into text, so this must stay a plain decimal
 * literal.
 */
#define WINDOW_DELAY_MS 250

/**
 * Window template: GUI with @p key, a pause of #WINDOW_DELAY_MS, then Ctrl with
 * @p key.
 *
 * @param key `X_*` key name from QMK's send string keycodes.
 */
#define GUI_THEN_CTRL(key) \
    SS_LGUI(SS_TAP(key)) SS_DELAY(WINDOW_DELAY_MS) SS_LCTL(SS_TAP(key))

/** Number of `ST_MACRO_*` keycodes, one row of #macro_sequences each. */
#define MACRO_COUNT (ST_MACRO_41 - ST_MACRO_38 + 1)

/** Bytes in each row of #macro_sequences: one sequence and its terminating NUL. */
#define MACRO_SEQUENCE_SIZE sizeof(GUI_THEN_CTRL(X_F1))

/**
 * Send string sequence of each `ST_MACRO_*` key, indexed by `keycode - ST_MACRO_38`.
 *
 * The rows live in program memory, so send_string_P() sends them.
 */
static const char PROGMEM macro_sequences[MACRO_COUNT][MACRO_SEQUENCE_SIZE] = {
    [ST_MACRO_38 - ST_MACRO_38] = GUI_THEN_CTRL(X_F1),
    [ST_MACRO_39 - ST_MACRO_38] = GUI_THEN_CTRL(X_F2),
    [ST_MACRO_40 - ST_MACRO_38] = GUI_THEN_CTRL(X_F3),
    [ST_MACRO_41 - ST_MACRO_38] = GUI_THEN_CTRL(X_F4),
};

bool process_macros(uint16_t keycode, keyrecord_t *record) {
    if (keycode < ST_MACRO_38 || keycode - ST_MACRO_38 >= MACRO_COUNT) {
        return true;
    }
    if (record->event.pressed) {
        send_string_P(macro_sequences[keycode - ST_MACRO_38]);
    }
    return true;
}
