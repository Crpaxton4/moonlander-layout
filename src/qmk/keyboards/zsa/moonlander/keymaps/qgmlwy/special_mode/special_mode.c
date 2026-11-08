/** @file
 * Mode key, one-shot and double whitespace end of the special mode.
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

/** True from the push that ends a one-shot until QMK has run its action */
static bool one_shot_ends_after_action;

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
 * Ends the special mode on a tap of the mode key if the mode is on, or starts a new
 * mode: a one-shot for a tap, a locked mode for a hold.
 *
 * @param record Key event of the mode key, with its tap count.
 */
static void process_mode_key(const keyrecord_t *record) {
    bool is_tap = record->tap.count > 0;
    if (!record->event.pressed) {
        return;
    } else if (is_tap && layer_state_is(LAYER_SPECIAL)) {
        layer_off(LAYER_SPECIAL);
        return;
    }
    special_mode_start(&mode, is_tap ? SPECIAL_MODE_ONE_SHOT : SPECIAL_MODE_LOCKED);
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
    enum special_mode_push_result result = special_mode_push(&mode, key_class(keycode));
    if (result == SPECIAL_MODE_STAYS_ON) {
        return true;
    } else if (result == SPECIAL_MODE_ENDS_AFTER_KEY) {
        // A layer_off() here gave the push the action of the base layer, for example
        // the GUI hold of T in place of %
        one_shot_ends_after_action = true;
        return true;
    }
    layer_off(LAYER_SPECIAL);
    // Bracket pairs ran first and counted the Space, which the host does not get
    bracket_pairs_take_back_count();
    return false;
}

void special_mode_count_taken_push(void) {
    if (!layer_state_is(LAYER_SPECIAL)) {
        return;
    }
    if (special_mode_push(&mode, SPECIAL_MODE_OTHER_KEY) != SPECIAL_MODE_STAYS_ON) {
        layer_off(LAYER_SPECIAL);
    }
}

void post_process_special_mode(uint16_t keycode, keyrecord_t *record) {
    (void)keycode;
    (void)record;
    if (!one_shot_ends_after_action) {
        return;
    }
    one_shot_ends_after_action = false;
    layer_off(LAYER_SPECIAL);
}

// #endregion
