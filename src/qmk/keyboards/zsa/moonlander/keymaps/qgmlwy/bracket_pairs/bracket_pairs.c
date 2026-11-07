/** @file
 * Key classes and QMK calls of the bracket and quote pairs.
 */

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "bracket_pairs/bracket_pairs_logic.h"
#include "layers/layers.h"
#include "special_mode/special_mode.h"
#include "symbol_holds/symbol_holds.h"

/** Open pairs; all zero at power-up, so the stack is empty */
static pair_stack_t pair_stack;

/** Opening and closing key of one pair kind */
typedef struct {
    uint16_t opener; /**< Key of the opening character */
    uint16_t closer; /**< Key of the closing character */
} pair_keys_t;

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

/** Number of bracket kinds, which come before the quotes in pair_keys */
#define BRACKET_KIND_COUNT PAIR_DOUBLE_QUOTES

/** Key of each fixed output, indexed by output; openers and closers use pair_keys */
static const uint16_t output_keys[] = {
    [PAIR_OUTPUT_LEFT]      = KC_LEFT,
    [PAIR_OUTPUT_RIGHT]     = KC_RIGHT,
    [PAIR_OUTPUT_BACKSPACE] = KC_BACKSPACE,
    [PAIR_OUTPUT_DELETE]    = KC_DELETE,
};

/** Most keys that the operator holds at one time with a push that this module took */
#define TAKEN_KEY_CAPACITY 4

/** Keys whose push this module took, KC_NO for a free place */
static uint16_t taken_keys[TAKEN_KEY_CAPACITY];

/** Range of keycodes with one key class */
typedef struct {
    uint16_t            first;     /**< First keycode of the range */
    uint16_t            last;      /**< Last keycode of the range */
    enum pair_key_class key_class; /**< Key class of each keycode in the range */
} key_range_t;

/** Key class of each range of basic keycodes; the first range that matches wins */
static const key_range_t key_ranges[] = {
    {.first = KC_TAB, .last = KC_TAB, .key_class = PAIR_KEY_TAB},
    {.first = KC_BACKSPACE, .last = KC_BACKSPACE, .key_class = PAIR_KEY_BACKSPACE},
    {.first = KC_ENTER, .last = KC_ESCAPE, .key_class = PAIR_KEY_CLEARING},
    {.first = KC_F1, .last = KC_F12, .key_class = PAIR_KEY_CLEARING},
    {.first = KC_F13, .last = KC_F24, .key_class = PAIR_KEY_CLEARING},
    // Insert, Home, Page Up, Delete, End, Page Down and the arrows
    {.first = KC_INSERT, .last = KC_UP, .key_class = PAIR_KEY_CLEARING},
    {
        .first     = QK_MOUSE_CURSOR_UP,
        .last      = QK_MOUSE_ACCELERATION_2,
        .key_class = PAIR_KEY_CLEARING,
    },
    // Letters, digits, Space and symbols; the ranges above match the other keys in it
    {.first = KC_A, .last = KC_SLASH, .key_class = PAIR_KEY_CHARACTER},
};

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

/** Modifiers that make a key push a shortcut, which can move the host cursor */
#define SHORTCUT_MODIFIERS (MOD_MASK_CTRL | MOD_MASK_ALT | MOD_MASK_GUI)

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
    } else if (key == KC_TAB && shifted) {
        // Shift with Tab moves the text, so the cursor can move
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
    for (uint8_t index = 0; index < result->output_count; index++) {
        tap_code16(key_of_output(result->outputs[index], result->kind));
    }
    if (shift != 0) {
        add_mods(shift);
    }
}

/**
 * Keeps a key whose push this module took, so that its release stops here too.
 *
 * @param keycode Keycode of the pushed key.
 */
static void remember_taken_key(uint16_t keycode) {
    for (uint8_t index = 0; index < TAKEN_KEY_CAPACITY; index++) {
        if (taken_keys[index] == KC_NO) {
            taken_keys[index] = keycode;
            return;
        }
    }
}

/**
 * Tells if a release goes on, and forgets the key if this module took its push.
 *
 * @param keycode Keycode of the released key.
 * @return False if this module took the push of the key, because QMK did not get
 *         that push; true otherwise.
 */
static bool release_goes_on(uint16_t keycode) {
    for (uint8_t index = 0; index < TAKEN_KEY_CAPACITY; index++) {
        if (keycode != KC_NO && taken_keys[index] == keycode) {
            taken_keys[index] = KC_NO;
            return false;
        }
    }
    return true;
}

bool process_bracket_pairs(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) {
        return release_goes_on(keycode);
    }
    pair_key_t    key    = classify_key(keycode, record);
    pair_result_t result = pair_stack_push(&pair_stack, key, timer_read32());
    if (result.passes) {
        return true;
    }
    send_outputs(&result);
    remember_taken_key(keycode);
    // Special mode runs later, so it does not see this push
    special_mode_count_taken_push();
    return false;
}

void bracket_pairs_clear(void) {
    pair_stack_clear(&pair_stack);
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
