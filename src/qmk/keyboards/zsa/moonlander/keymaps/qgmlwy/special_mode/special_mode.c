/** @file
 * Mode key and double whitespace end of the special mode.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "special_mode/special_mode.h"
#include "special_mode/special_mode_logic.h"

// #endregion

// #region Types

/** Keycode of one whitespace key and its key class */
typedef struct {
    uint16_t              keycode; /**< Keycode on LAYER_SPECIAL */
    enum special_mode_key key;     /**< Key class for the double whitespace rule */
} whitespace_key_t;

// #endregion

// #region Data

/** Whitespace keys of LAYER_SPECIAL; Backspace is not one of them */
static const whitespace_key_t whitespace_keys[] = {
    {.keycode = KC_SPACE, .key = SPECIAL_MODE_SPACE},
    {.keycode = KC_ENTER, .key = SPECIAL_MODE_ENTER},
    {.keycode = KC_TAB, .key = SPECIAL_MODE_TAB},
};

/** State of the mode that is on, or of the last mode */
static special_mode_t mode;

// #endregion

// #region Static functions

/**
 * Gives the key class of a keycode.
 *
 * @param keycode Keycode of a push on LAYER_SPECIAL.
 * @return Class of the whitespace key, or SPECIAL_MODE_OTHER_KEY.
 */
static enum special_mode_key key_class(uint16_t keycode) {
    for (size_t index = 0; index < ARRAY_SIZE(whitespace_keys); index++) {
        if (whitespace_keys[index].keycode == keycode) {
            return whitespace_keys[index].key;
        }
    }
    return SPECIAL_MODE_OTHER_KEY;
}

/**
 * Ends the special mode on a push of the mode key if the mode is on, or starts a
 * new mode.
 *
 * @param record Key event of the mode key.
 */
static void process_mode_key(const keyrecord_t *record) {
    if (!record->event.pressed) {
        return;
    }
    if (layer_state_is(LAYER_SPECIAL)) {
        layer_off(LAYER_SPECIAL);
        return;
    }
    special_mode_start(&mode);
    layer_on(LAYER_SPECIAL);
}

// #endregion

// #region Public functions

bool process_special_mode(uint16_t keycode, keyrecord_t *record) {
    if (keycode == SPECIAL_MODE_KEY) {
        process_mode_key(record);
        return false;
    }
    if (!record->event.pressed || !layer_state_is(LAYER_SPECIAL)) {
        return true;
    }
    if (!special_mode_push_ends_mode(&mode, key_class(keycode))) {
        return true;
    }
    layer_off(LAYER_SPECIAL);
    // Bracket pairs ran first and counted the Space, which the host does not get
    bracket_pairs_take_back_count();
    return false;
}

void special_mode_count_taken_push(void) {
    (void)special_mode_push_ends_mode(&mode, SPECIAL_MODE_OTHER_KEY);
}

// #endregion
