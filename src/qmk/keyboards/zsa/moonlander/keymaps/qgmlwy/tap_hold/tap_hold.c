/** @file
 * QMK tap-hold callbacks of the qgmlwy keymap.
 *
 * QMK declares these callbacks, and config.h sets the features that call them.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "layers/layers.h"

// #endregion

// #region Enumerations

/** Hands of chordal hold, with the characters that QMK uses for them */
enum hand {
    LEFT_HAND   = 'L', /**< Key of the left hand */
    RIGHT_HAND  = 'R', /**< Key of the right hand */
    EITHER_HAND = '*', /**< Key that combines with keys of each hand */
};

// #endregion

// #region Data

/**
 * Hand of the keys of each matrix row. Thumb keys combine with each hand, so a
 * home row key and a thumb key give the modifier.
 */
static const char hand_of_row[MATRIX_ROWS] = {
    [LEFT_NUMBER_ROW] = LEFT_HAND,   [LEFT_TOP_ROW] = LEFT_HAND,
    [LEFT_HOME_ROW] = LEFT_HAND,     [LEFT_BOTTOM_LETTER_ROW] = LEFT_HAND,
    [LEFT_BOTTOM_ROW] = LEFT_HAND,   [LEFT_THUMB_KEYS] = EITHER_HAND,
    [RIGHT_NUMBER_ROW] = RIGHT_HAND, [RIGHT_TOP_ROW] = RIGHT_HAND,
    [RIGHT_HOME_ROW] = RIGHT_HAND,   [RIGHT_BOTTOM_LETTER_ROW] = RIGHT_HAND,
    [RIGHT_BOTTOM_ROW] = RIGHT_HAND, [RIGHT_THUMB_KEYS] = EITHER_HAND,
};

// #endregion

// #region Public functions

/**
 * Gives the hand of a key for chordal hold.
 *
 * Moonlander's own table in moonlander.c gives each thumb key one hand, so this
 * function replaces QMK's lookup in that table.
 *
 * @param key Matrix position of the key.
 * @return Hand of the key.
 */
char chordal_hold_handedness(keypos_t key) {
    return hand_of_row[key.row];
}

/**
 * Gives the tapping term of a tap-hold key.
 *
 * @param keycode Keycode of the tap-hold key.
 * @param record  Push of the key; unused.
 * @return SHIFT_TAPPING_TERM for a Shift mod-tap key, TAPPING_TERM for other keys.
 */
uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
    (void)record;
    uint8_t modifiers = QK_MOD_TAP_GET_MODS(keycode);
    if (IS_QK_MOD_TAP(keycode) && (modifiers == MOD_LSFT || modifiers == MOD_RSFT)) {
        return SHIFT_TAPPING_TERM;
    }
    return TAPPING_TERM;
}

/**
 * Gives the flow tap term of a tap-hold key after a prior key.
 *
 * Only a letter starts the term, so a fast Shift after a space or a period gives a
 * capital letter. QMK's default stops the term while Ctrl, Alt or GUI is held. Here
 * the term also applies in a shortcut, so a tap-hold key taps in the same way.
 *
 * @param keycode   Keycode of the tap-hold key; unused, because all tap-hold keys
 *                  use the term.
 * @param record    Push of the tap-hold key; unused.
 * @param prior_key Keycode of the key before the tap-hold key.
 * @return FLOW_TAP_TERM after a key with a letter as its tap key; 0 after other
 *         keys.
 */
uint16_t get_flow_tap_term(uint16_t keycode, keyrecord_t *record, uint16_t prior_key) {
    (void)record;
    // A tap that ends late and a quick push again of the same key must not repeat
    // the letter: the usual tap-hold decision then gives the hold
    if (keycode == prior_key) {
        return 0;
    }
    uint16_t prior_tap_key = prior_key;
    // Mod-tap and layer-tap keys both keep their tap key in the low byte
    if (IS_QK_MOD_TAP(prior_key) || IS_QK_LAYER_TAP(prior_key)) {
        prior_tap_key = QK_MOD_TAP_GET_TAP_KEYCODE(prior_key);
    }
    if (prior_tap_key >= KC_A && prior_tap_key <= KC_Z) {
        return FLOW_TAP_TERM;
    }
    return 0;
}

// #endregion
