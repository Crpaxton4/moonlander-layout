/** @file
 * Key sequences of the qgmlwy keymap's `ST_MACRO_*` keys: one table row per macro,
 * built from two templates, and the lookup that sends a row.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "macros/macros.h"

/**
 * Milliseconds the launcher macros wait between the launcher chord and their second
 * chord.
 *
 * `SS_DELAY()` turns its argument into text, so this must stay a plain decimal
 * literal.
 */
#define LAUNCHER_DELAY_MS 100

/**
 * Milliseconds the window macros wait between their GUI and Ctrl chords.
 *
 * `SS_DELAY()` turns its argument into text, so this must stay a plain decimal
 * literal.
 */
#define WINDOW_DELAY_MS 250

/**
 * Sequence that taps @p key with Alt, Ctrl and Shift held.
 *
 * @param key `X_*` key name from QMK's send string keycodes.
 */
#define ALT_CTRL_SHIFT(key) SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(key))))

/**
 * Sequence that taps @p key with Alt and Ctrl held.
 *
 * @param key `X_*` key name from QMK's send string keycodes.
 */
#define ALT_CTRL(key) SS_LALT(SS_LCTL(SS_TAP(key)))

/** Launcher chord that opens the launcher: Alt+Ctrl+Shift+grave. */
#define LAUNCHER_CHORD ALT_CTRL_SHIFT(X_GRAVE)

/**
 * Launcher template: the launcher chord, a pause of #LAUNCHER_DELAY_MS, then
 * @p chord.
 *
 * @param chord Send string sequence of the second chord.
 */
#define LAUNCHER_THEN(chord) LAUNCHER_CHORD SS_DELAY(LAUNCHER_DELAY_MS) chord

/**
 * Window template: GUI with @p key, a pause of #WINDOW_DELAY_MS, then Ctrl with
 * @p key.
 *
 * @param key `X_*` key name from QMK's send string keycodes.
 */
#define GUI_THEN_CTRL(key) \
    SS_LGUI(SS_TAP(key)) SS_DELAY(WINDOW_DELAY_MS) SS_LCTL(SS_TAP(key))

/** Number of `ST_MACRO_*` keycodes, one row of #macro_sequences each. */
#define MACRO_COUNT (ST_MACRO_41 - ST_MACRO_0 + 1)

/**
 * Bytes in each row of #macro_sequences: the longest sequence, a launcher macro
 * with an Alt+Ctrl+Shift second chord, plus its terminating NUL.
 */
#define MACRO_SEQUENCE_SIZE sizeof(LAUNCHER_THEN(ALT_CTRL_SHIFT(X_GRAVE)))

/**
 * Send string sequence of each `ST_MACRO_*` key, indexed by `keycode - ST_MACRO_0`.
 *
 * `ST_MACRO_0` to `ST_MACRO_37` use the launcher template and `ST_MACRO_38` to
 * `ST_MACRO_41` the window template. The rows live in program memory, so
 * send_string_P() sends them.
 */
static const char PROGMEM macro_sequences[MACRO_COUNT][MACRO_SEQUENCE_SIZE] = {
    [ST_MACRO_0 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_1)),
    [ST_MACRO_1 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_2)),
    [ST_MACRO_2 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_X)),
    [ST_MACRO_3 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_Q)),
    [ST_MACRO_4 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_EQUAL)),
    [ST_MACRO_5 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_E)),
    [ST_MACRO_6 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_R)),
    [ST_MACRO_7 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_T)),
    [ST_MACRO_8 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_C)),
    [ST_MACRO_9 - ST_MACRO_0]  = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_A)),
    [ST_MACRO_10 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_S)),
    [ST_MACRO_11 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_D)),
    [ST_MACRO_12 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_F)),
    [ST_MACRO_13 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_G)),
    [ST_MACRO_14 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_V)),
    [ST_MACRO_15 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_Z)),
    [ST_MACRO_16 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_SLASH)),
    [ST_MACRO_17 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_B)),
    /* The one launcher macro whose second chord has no Shift. */
    [ST_MACRO_18 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL(X_N)),
    [ST_MACRO_19 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_Y)),
    [ST_MACRO_20 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_U)),
    [ST_MACRO_21 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_I)),
    [ST_MACRO_22 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_O)),
    [ST_MACRO_23 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_P)),
    [ST_MACRO_24 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_BSLS)),
    [ST_MACRO_25 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_N)),
    [ST_MACRO_26 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_H)),
    [ST_MACRO_27 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_J)),
    [ST_MACRO_28 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_K)),
    [ST_MACRO_29 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_L)),
    [ST_MACRO_30 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_SCLN)),
    [ST_MACRO_31 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_RBRC)),
    [ST_MACRO_32 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_GRAVE)),
    [ST_MACRO_33 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_LEFT)),
    [ST_MACRO_34 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_UP)),
    [ST_MACRO_35 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_RIGHT)),
    [ST_MACRO_36 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_DOWN)),
    [ST_MACRO_37 - ST_MACRO_0] = LAUNCHER_THEN(ALT_CTRL_SHIFT(X_LBRC)),
    [ST_MACRO_38 - ST_MACRO_0] = GUI_THEN_CTRL(X_F1),
    [ST_MACRO_39 - ST_MACRO_0] = GUI_THEN_CTRL(X_F2),
    [ST_MACRO_40 - ST_MACRO_0] = GUI_THEN_CTRL(X_F3),
    [ST_MACRO_41 - ST_MACRO_0] = GUI_THEN_CTRL(X_F4),
};

bool process_macros(uint16_t keycode, keyrecord_t *record) {
    if (keycode < ST_MACRO_0 || keycode - ST_MACRO_0 >= MACRO_COUNT) {
        return true;
    }
    if (record->event.pressed) {
        send_string_P(macro_sequences[keycode - ST_MACRO_0]);
    }
    return true;
}
