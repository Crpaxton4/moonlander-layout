/** @file
 * Key classes and QMK calls of the case modes.
 */

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "case_modes/case_modes.h"
#include "case_modes/case_modes_logic.h"
#include "keycodes/keycodes.h"

/** Case mode state; all zero at power-up, so no mode is on */
static case_modes_t case_modes;

/** Mode of one mode key */
typedef struct {
    uint16_t       keycode; /**< Keycode of the mode key */
    enum case_mode mode;    /**< Mode that the key starts */
} mode_key_t;

/** Mode of each mode key */
static const mode_key_t mode_keys[] = {
    {SNAKE_CASE_KEY, CASE_MODE_SNAKE},
    {CONSTANT_CASE_KEY, CASE_MODE_CONSTANT},
    {CAMEL_CASE_KEY, CASE_MODE_CAMEL},
    {PASCAL_CASE_KEY, CASE_MODE_PASCAL},
};

/**
 * Gives the mode of a mode key.
 *
 * @param keycode Keycode of the pushed key.
 * @return Mode of the key, or CASE_MODE_OFF for a key that is not a mode key.
 */
static enum case_mode mode_of_key(uint16_t keycode) {
    for (size_t index = 0; index < ARRAY_SIZE(mode_keys); index++) {
        if (mode_keys[index].keycode == keycode) {
            return mode_keys[index].mode;
        }
    }
    return CASE_MODE_OFF;
}

/**
 * Gives the key that a push sends: the tap key for the tap of a mod-tap or
 * layer-tap key, the keycode itself else.
 *
 * @param keycode Keycode of the pushed key.
 * @param record  Key event, with its tap count.
 * @return Keycode of the sent key.
 */
static uint16_t sent_key(uint16_t keycode, const keyrecord_t *record) {
    bool is_tap_hold = IS_QK_MOD_TAP(keycode) || IS_QK_LAYER_TAP(keycode);
    if (!is_tap_hold || record->tap.count == 0) {
        return keycode;
    }
    // Mod-tap and layer-tap keycodes both keep the tap key in their low byte
    return QK_MOD_TAP_GET_TAP_KEYCODE(keycode);
}

/** Modifiers that make a key push a shortcut, not text */
#define SHORTCUT_MODIFIERS (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)

/**
 * Tells if a push only holds a modifier, which types nothing.
 *
 * @param key    Sent key, from sent_key().
 * @param record Key event, with its tap count.
 * @return True for a modifier key or the hold of a mod-tap key.
 */
static bool is_modifier_push(uint16_t key, const keyrecord_t *record) {
    return IS_MODIFIER_KEYCODE(key) || (IS_QK_MOD_TAP(key) && record->tap.count == 0);
}

/**
 * Gives the key class of a key that types text.
 *
 * @param key Sent key, from sent_key().
 * @return Key class; a digit with Shift types a symbol, so it is CASE_KEY_OTHER.
 */
static enum case_key_class class_of_text_key(uint16_t key) {
    bool shifted = (get_mods() & MOD_MASK_SHIFT) != 0;
    if (key >= KC_A && key <= KC_Z) {
        return CASE_KEY_LETTER;
    } else if (key >= KC_1 && key <= KC_0 && !shifted) {
        return CASE_KEY_DIGIT;
    } else if (key == KC_SPACE) {
        return CASE_KEY_SPACE;
    } else if (key == KC_BACKSPACE) {
        return CASE_KEY_BACKSPACE;
    }
    return CASE_KEY_OTHER;
}

/**
 * Gives the key class of a pushed key.
 *
 * @param key    Sent key, from sent_key().
 * @param record Key event, with its tap count.
 * @return Key class, with the mode of a mode key.
 */
static case_key_t classify_key(uint16_t key, const keyrecord_t *record) {
    enum case_mode mode = mode_of_key(key);
    if (mode != CASE_MODE_OFF) {
        return (case_key_t){.key_class = CASE_KEY_MODE, .mode = mode};
    } else if (is_modifier_push(key, record)) {
        return (case_key_t){.key_class = CASE_KEY_MODIFIER};
    } else if ((get_mods() & SHORTCUT_MODIFIERS) != 0) {
        return (case_key_t){.key_class = CASE_KEY_OTHER};
    }
    return (case_key_t){.key_class = class_of_text_key(key)};
}

/**
 * Sends a key in place of the pushed key, and counts it in the bracket pairs.
 *
 * @param keycode Key to send.
 */
static void send_key(uint16_t keycode) {
    tap_code16(keycode);
    bracket_pairs_count_sent_key(keycode);
}

/**
 * Lets the pushed key pass.
 *
 * @param keycode Keycode of the pushed key.
 * @return True, so QMK sends the key.
 */
static bool send_as_is(uint16_t keycode) {
    (void)keycode;
    return true;
}

/**
 * Sends the pushed key with Shift in place of the key.
 *
 * @param keycode Keycode of the pushed key.
 * @return False, so QMK does not also send the key.
 */
static bool send_with_shift(uint16_t keycode) {
    send_key(LSFT(keycode));
    return false;
}

/**
 * Sends an underscore in place of the pushed key.
 *
 * @param keycode Keycode of the pushed key.
 * @return False, so QMK does not also send the key.
 */
static bool send_underscore(uint16_t keycode) {
    (void)keycode;
    send_key(KC_UNDERSCORE);
    return false;
}

/**
 * Sends no key for the push.
 *
 * @param keycode Keycode of the pushed key.
 * @return False, so QMK does not send the key.
 */
static bool send_nothing(uint16_t keycode) {
    (void)keycode;
    return false;
}

/**
 * Sends Backspace, then lets the pushed Space pass, which ends a mode on a double
 * Space with one Space and no underscore.
 *
 * @param keycode Keycode of the pushed key.
 * @return True, so QMK sends the Space.
 */
static bool send_backspace_then_space(uint16_t keycode) {
    (void)keycode;
    send_key(KC_BACKSPACE);
    return true;
}

/**
 * Does one action of the case modes.
 *
 * @param keycode Keycode of the pushed key.
 * @return True if QMK must send the pushed key.
 */
typedef bool (*action_handler_t)(uint16_t keycode);

/** Handler of each action, indexed by action */
static const action_handler_t action_handlers[] = {
    [CASE_ACTION_SEND_AS_IS]                = send_as_is,
    [CASE_ACTION_SEND_WITH_SHIFT]           = send_with_shift,
    [CASE_ACTION_SEND_UNDERSCORE]           = send_underscore,
    [CASE_ACTION_SEND_NOTHING]              = send_nothing,
    [CASE_ACTION_SEND_BACKSPACE_THEN_SPACE] = send_backspace_then_space,
};

bool process_case_modes(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return true;
    }
    uint16_t         key    = sent_key(keycode, record);
    enum case_action action = case_modes_push(&case_modes, classify_key(key, record));
    return action_handlers[action](key);
}

void case_modes_end_all(void) {
    case_modes_end(&case_modes);
}
