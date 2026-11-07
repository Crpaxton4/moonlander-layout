/** @file
 * Key classes and QMK calls of the bracket and quote pairs.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "bracket_pairs/bracket_pairs_logic.h"
#include "layers/layers.h"
#include "special_mode/special_mode.h"
#include "symbol_holds/symbol_holds.h"

// #endregion

// #region Macros

/** Number of bracket kinds, which come before the quotes in pair_keys */
#define BRACKET_KIND_COUNT PAIR_DOUBLE_QUOTES

/** Most keys that the operator holds at one time with a push that this module saw */
#define HELD_KEY_CAPACITY 8

/** Modifiers that make a key push a shortcut, which can move the host cursor */
#define SHORTCUT_MODIFIERS (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)

// #endregion

// #region Types

/** Opening and closing key of one pair kind */
typedef struct {
    uint16_t opener; /**< Key of the opening character */
    uint16_t closer; /**< Key of the closing character */
} pair_keys_t;

/** Key that the operator holds, with a push that this module saw */
typedef struct {
    uint16_t keycode;   /**< Keycode of the key, KC_NO for a free place */
    bool     is_taken;  /**< True if this module took the push, so QMK did not get it */
    uint32_t push_time; /**< Time of the push at the hook in ms */
} held_key_t;

/** Range of keycodes with one key class */
typedef struct {
    uint16_t            first;     /**< First keycode of the range */
    uint16_t            last;      /**< Last keycode of the range */
    enum pair_key_class key_class; /**< Key class of each keycode in the range */
} key_range_t;

// #endregion

// #region Data

/** Open pairs; all zero at power-up, so the stack is empty */
static pair_stack_t pair_stack;

/** Keys of each pair kind for a US English host layout, indexed by kind */
static const pair_keys_t pair_keys[] = {
    [PAIR_PARENTHESES] =
        {
            .opener = KC_LEFT_PAREN,
            .closer = KC_RIGHT_PAREN,
        },
    [PAIR_SQUARE_BRACKETS] =
        {
            .opener = KC_LEFT_BRACKET,
            .closer = KC_RIGHT_BRACKET,
        },
    [PAIR_CURLY_BRACES] =
        {
            .opener = KC_LEFT_CURLY_BRACE,
            .closer = KC_RIGHT_CURLY_BRACE,
        },
    [PAIR_ANGLE_BRACKETS] =
        {
            .opener = KC_LEFT_ANGLE_BRACKET,
            .closer = KC_RIGHT_ANGLE_BRACKET,
        },
    [PAIR_DOUBLE_QUOTES] =
        {
            .opener = KC_DOUBLE_QUOTE,
            .closer = KC_DOUBLE_QUOTE,
        },
    [PAIR_BACKTICKS] =
        {
            .opener = KC_GRAVE,
            .closer = KC_GRAVE,
        },
};

/** Key of each fixed output, indexed by output; openers and closers use pair_keys */
static const uint16_t output_keys[] = {
    [PAIR_OUTPUT_LEFT]      = KC_LEFT,
    [PAIR_OUTPUT_RIGHT]     = KC_RIGHT,
    [PAIR_OUTPUT_BACKSPACE] = KC_BACKSPACE,
    [PAIR_OUTPUT_DELETE]    = KC_DELETE,
};

/** Held keys with a push that this module took or that can change the host text */
static held_key_t held_keys[HELD_KEY_CAPACITY];

/** Keys that move the cursor or the text with Shift held */
static const uint16_t shift_move_keys[] = {
    KC_TAB,
    KC_LEFT,
    KC_RIGHT,
};

/** Key class of each range of basic keycodes; the first range that matches wins */
static const key_range_t key_ranges[] = {
    {.first = KC_TAB, .last = KC_TAB, .key_class = PAIR_KEY_TAB},
    {.first = KC_BACKSPACE, .last = KC_BACKSPACE, .key_class = PAIR_KEY_BACKSPACE},
    {.first = KC_ENTER, .last = KC_ESCAPE, .key_class = PAIR_KEY_CLEARING},
    {.first = KC_F1, .last = KC_F12, .key_class = PAIR_KEY_CLEARING},
    {.first = KC_F13, .last = KC_F24, .key_class = PAIR_KEY_CLEARING},
    {.first = KC_RIGHT, .last = KC_RIGHT, .key_class = PAIR_KEY_RIGHT},
    {.first = KC_LEFT, .last = KC_LEFT, .key_class = PAIR_KEY_LEFT},
    // Insert, Home, Page Up, Delete, End, Page Down, Down and Up
    {.first = KC_INSERT, .last = KC_UP, .key_class = PAIR_KEY_CLEARING},
    {
        .first     = QK_MOUSE_CURSOR_UP,
        .last      = QK_MOUSE_ACCELERATION_2,
        .key_class = PAIR_KEY_CLEARING,
    },
    // Letters, digits, Space and symbols; the ranges above match the other keys in it
    {.first = KC_A, .last = KC_SLASH, .key_class = PAIR_KEY_CHARACTER},
};

// #endregion

// #region Static functions

/**
 * Gives the key class of a key that QMK sends, from its basic keycode.
 *
 * @param key Keycode, with or without modifier bits.
 * @return Key class from key_ranges, or PAIR_KEY_NO_CHARACTER.
 */
static enum pair_key_class class_of_key(uint16_t key) {
    uint16_t basic_key = IS_QK_MODS(key) ? QK_MODS_GET_BASIC_KEYCODE(key) : key;
    for (size_t index = 0; index < ARRAY_SIZE(key_ranges); index++) {
        if (basic_key >= key_ranges[index].first &&
            basic_key <= key_ranges[index].last) {
            return key_ranges[index].key_class;
        }
    }
    return PAIR_KEY_NO_CHARACTER;
}

/**
 * Gives the pair key of the symbol of a symbol hold.
 *
 * @param symbol Symbol of the hold, from symbol_of_hold_key().
 * @return Opener or closer with its kind, PAIR_KEY_CHARACTER for another symbol, or
 *         PAIR_KEY_NO_CHARACTER for KC_NO.
 */
static pair_key_t class_of_symbol(uint16_t symbol) {
    for (size_t kind = 0; kind < BRACKET_KIND_COUNT; kind++) {
        if (pair_keys[kind].opener == symbol) {
            return (pair_key_t){.key_class = PAIR_KEY_OPENER,
                                .kind      = (enum pair_kind)kind};
        } else if (pair_keys[kind].closer == symbol) {
            return (pair_key_t){.key_class = PAIR_KEY_CLOSER,
                                .kind      = (enum pair_kind)kind};
        }
    }
    return (pair_key_t){.key_class = class_of_key(symbol)};
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

/**
 * Gives the pair key of a symbol hold.
 *
 * @param keycode Keycode of the held key.
 * @param shifted True while a Shift is held.
 * @return Pair key of the hold; with Shift, an opener makes no pair.
 */
static pair_key_t classify_hold(uint16_t keycode, bool shifted) {
    pair_key_t key = class_of_symbol(symbol_of_hold_key(keycode));
    if (key.key_class == PAIR_KEY_OPENER && shifted) {
        key.key_class = PAIR_KEY_LONE_OPENER;
    }
    return key;
}

/**
 * Tells if a push only holds a modifier, which types nothing.
 *
 * @param keycode Keycode of the pushed key.
 * @param record  Key event, with its tap count.
 * @return True for a modifier key or the hold of a mod-tap key.
 */
static bool is_modifier_push(uint16_t keycode, const keyrecord_t *record) {
    bool is_mod_tap_hold = IS_QK_MOD_TAP(keycode) && record->tap.count == 0;
    return IS_MODIFIER_KEYCODE(keycode) || is_mod_tap_hold;
}

/**
 * Tells if a key moves the cursor or the text with Shift held.
 *
 * @param key Sent key, from sent_key().
 * @return True for a key in shift_move_keys.
 */
static bool is_shift_move_key(uint16_t key) {
    for (size_t index = 0; index < ARRAY_SIZE(shift_move_keys); index++) {
        if (shift_move_keys[index] == key) {
            return true;
        }
    }
    return false;
}

/**
 * Gives the pair key of a key that QMK sends for a push.
 *
 * @param key     Sent key, from sent_key().
 * @param shifted True while a Shift is held.
 * @return Pair key of the push.
 */
static pair_key_t classify_sent_key(uint16_t key, bool shifted) {
    if (key == KC_QUOTE && shifted) {
        return (pair_key_t){.key_class = PAIR_KEY_QUOTE, .kind = PAIR_DOUBLE_QUOTES};
    } else if (key == KC_GRAVE && !shifted && layer_state_is(LAYER_SPECIAL)) {
        return (pair_key_t){.key_class = PAIR_KEY_QUOTE, .kind = PAIR_BACKTICKS};
    } else if (shifted && is_shift_move_key(key)) {
        // Shift with Tab moves the text, and Shift with an arrow selects text
        return (pair_key_t){.key_class = PAIR_KEY_CLEARING};
    }
    return (pair_key_t){.key_class = class_of_key(key)};
}

/**
 * Gives the pair key of a pushed key.
 *
 * @param keycode Keycode of the pushed key.
 * @param record  Key event, with its tap count.
 * @return Pair key of the push.
 */
static pair_key_t classify_key(uint16_t keycode, const keyrecord_t *record) {
    uint8_t mods    = get_mods();
    bool    is_hold = IS_QK_LAYER_TAP(keycode) && record->tap.count == 0;
    if (is_modifier_push(keycode, record)) {
        return (pair_key_t){.key_class = PAIR_KEY_NO_CHARACTER};
    } else if ((mods & SHORTCUT_MODIFIERS) != 0) {
        return (pair_key_t){.key_class = PAIR_KEY_CLEARING};
    } else if (is_hold) {
        return classify_hold(keycode, (mods & MOD_MASK_SHIFT) != 0);
    }
    return classify_sent_key(sent_key(keycode, record), (mods & MOD_MASK_SHIFT) != 0);
}

/**
 * Gives the key of one output.
 *
 * @param output Output of the pair stack.
 * @param kind   Pair kind of an opener or closer output.
 * @return Keycode to tap.
 */
static uint16_t key_of_output(enum pair_output output, enum pair_kind kind) {
    if (output == PAIR_OUTPUT_OPENER) {
        return pair_keys[kind].opener;
    } else if (output == PAIR_OUTPUT_CLOSER) {
        return pair_keys[kind].closer;
    }
    return output_keys[output];
}

/**
 * Taps each output key of a result, with the held Shift off.
 *
 * @param result Result of the push.
 */
static void send_outputs(const pair_result_t *result) {
    // With Shift, the arrows select text and Delete cuts it
    uint8_t shift = get_mods() & MOD_MASK_SHIFT;
    if (shift != 0) {
        del_mods(shift);
    }
    // Wider than repeat_count, so the loop ends for each value
    for (uint32_t send = 0; send <= result->repeat_count; send++) {
        for (uint8_t index = 0; index < result->output_count; index++) {
            tap_code16(key_of_output(result->outputs[index], result->kind));
        }
    }
    if (shift != 0) {
        add_mods(shift);
    }
}

/**
 * Gives the place of a held key.
 *
 * @param keycode Keycode of the key, or KC_NO for a free place.
 * @return Place in held_keys, or NULL if no place has the keycode.
 */
static held_key_t *find_held_key(uint16_t keycode) {
    for (uint8_t index = 0; index < HELD_KEY_CAPACITY; index++) {
        if (held_keys[index].keycode == keycode) {
            return &held_keys[index];
        }
    }
    return NULL;
}

/**
 * Keeps a pushed key until its release. With no free place, the release goes on
 * with no check.
 *
 * @param keycode  Keycode of the pushed key.
 * @param is_taken True if this module took the push.
 */
static void remember_held_key(uint16_t keycode, bool is_taken) {
    held_key_t *place = find_held_key(keycode);
    if (place == NULL) {
        place = find_held_key(KC_NO);
    }
    if (place == NULL || keycode == KC_NO) {
        return;
    }
    *place = (held_key_t){
        .keycode   = keycode,
        .is_taken  = is_taken,
        .push_time = timer_read32(),
    };
}

/**
 * Forgets a released key. Clears the stack if the host can have repeated the key.
 *
 * @param keycode Keycode of the released key.
 * @return False if this module took the push of the key, because QMK did not get
 *         that push; true otherwise.
 */
static bool release_goes_on(uint16_t keycode) {
    held_key_t *held = find_held_key(keycode);
    if (held == NULL || keycode == KC_NO) {
        return true;
    }
    held->keycode = KC_NO;
    if (held->is_taken) {
        return false;
    }
    pair_stack_release_passed_key(&pair_stack, timer_elapsed32(held->push_time));
    return true;
}

// #endregion

// #region Public functions

bool process_bracket_pairs(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return release_goes_on(keycode);
    }
    pair_key_t    key    = classify_key(keycode, record);
    pair_result_t result = pair_stack_push(&pair_stack, key, timer_read32());
    if (result.passes && key.key_class == PAIR_KEY_NO_CHARACTER) {
        // Host does not repeat a modifier or a layer key
        return true;
    } else if (result.passes) {
        remember_held_key(keycode, false);
        return true;
    }
    send_outputs(&result);
    remember_held_key(keycode, true);
    // Special mode runs later, so it does not see this push
    special_mode_count_taken_push();
    return false;
}

void bracket_pairs_clear(void) {
    pair_stack_clear(&pair_stack);
}

bool bracket_pairs_are_open(void) {
    return pair_stack_has_pairs(&pair_stack, timer_read32());
}

void bracket_pairs_count_sent_key(uint16_t keycode) {
    pair_key_t sent = {.key_class = class_of_key(keycode)};
    pair_stack_count_sent_key(&pair_stack, sent, timer_read32());
}

void bracket_pairs_take_back_count(void) {
    // Same count change as a Backspace, because the push counted one character
    pair_key_t taken_back = {.key_class = PAIR_KEY_BACKSPACE};
    pair_stack_count_sent_key(&pair_stack, taken_back, timer_read32());
}

// #endregion
