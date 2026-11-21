/** @file
 * Name table of the QMK keycodes that the qgmlwy keymap sends to the key event log
 */

// #region Includes

#include <stdio.h>

#include "hid_usage.h"
#include "keycode_name.h"
#include "support.h"

// #endregion

// #region Macros

/** Bytes of the longest tap key name, with its terminator */
#define TAP_KEY_NAME_SIZE 16

/** Bits of the tap key in a mod-tap, layer-tap or modified keycode */
#define TAP_KEY_MASK 0x00FFU

/** Bits above the tap key in a mod-tap, layer-tap or modified keycode */
#define HOLD_SHIFT 8U

/** Ctrl, Shift, Alt and GUI bits of a mod-tap or modified keycode, after HOLD_SHIFT */
#define MODIFIER_SET_MASK 0x0FU

/** Bit of a mod-tap keycode that selects the right modifiers */
#define RIGHT_MODIFIERS_BIT 0x1000U

/** Layer bits of a layer-tap keycode, after HOLD_SHIFT */
#define HOLD_LAYER_MASK 0x0FU

/** Layer bits of a layer toggle keycode */
#define TOGGLE_LAYER_MASK 0x1FU

// #endregion

// #region Types

/** Keycode that has its own name */
typedef struct {
    uint16_t    keycode; /**< QMK keycode */
    const char *name;    /**< Name in the report */
} named_keycode_t;

/**
 * Writes the name of a keycode of one range.
 *
 * @param keycode Keycode in the range.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
typedef void range_writer_t(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);

/** Range of QMK keycodes that have a key or a layer in their bits */
typedef struct {
    uint16_t        first; /**< First keycode of the range */
    uint16_t        last;  /**< Last keycode of the range */
    range_writer_t *write; /**< Writer of the names of the range */
} keycode_range_t;

// #endregion

// #region Function declarations

static void write_modified_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);
static void write_mod_tap_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);
static void write_layer_tap_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);
static void write_toggle_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]);

// #endregion

// #region Data

/**
 * Keycodes with their own name: no key, the mouse keys, the modifiers, and the
 * keycodes of the keymap. The keymap gives its own keycodes the values from
 * 0x7E40, in the order of its enumeration.
 */
static const named_keycode_t named_keycodes[] = {
    {0x0000, "No key"},
    {0x00CD, "Mouse up"},
    {0x00CE, "Mouse down"},
    {0x00CF, "Mouse left"},
    {0x00D0, "Mouse right"},
    {0x00D1, "Mouse button 1"},
    {0x00D2, "Mouse button 2"},
    {0x00D3, "Mouse button 3"},
    {0x00D4, "Mouse button 4"},
    {0x00D5, "Mouse button 5"},
    {0x00D6, "Mouse button 6"},
    {0x00D7, "Mouse button 7"},
    {0x00D8, "Mouse button 8"},
    {0x00D9, "Wheel up"},
    {0x00DA, "Wheel down"},
    {0x00DB, "Wheel left"},
    {0x00DC, "Wheel right"},
    {0x00DD, "Mouse speed 0"},
    {0x00DE, "Mouse speed 1"},
    {0x00DF, "Mouse speed 2"},
    {0x00E0, "Left Ctrl"},
    {0x00E1, "Left Shift"},
    {0x00E2, "Left Alt"},
    {0x00E3, "Left GUI"},
    {0x00E4, "Right Ctrl"},
    {0x00E5, "Right Shift"},
    {0x00E6, "Right Alt"},
    {0x00E7, "Right GUI"},
    {0x4000, "Special mode"},
    {0x403F, "Snake case"},
    {0x4040, "Camel case"},
    {0x7E40, "Logging combo left"},
    {0x7E41, "Logging combo right"},
    {0x7E42, "Logging toggle"},
    {0x7E43, "Constant case"},
    {0x7E44, "Pascal case"},
    {0x7E45, "Plain mode combo left"},
    {0x7E46, "Plain mode combo right"},
    {0x7E47, "Plain mode toggle"},
    {0x7E48, "Profiling toggle"},
};

/** Keycode ranges of QMK */
static const keycode_range_t keycode_ranges[] = {
    {0x0100, 0x1FFF, write_modified_name},
    {0x2000, 0x3FFF, write_mod_tap_name},
    {0x4000, 0x4FFF, write_layer_tap_name},
    {0x5260, 0x527F, write_toggle_name},
};

/** Names of the modifier sets, indexed by the Ctrl, Shift, Alt and GUI bits */
static const char *const modifier_set_names[] = {
    "",        "Ctrl",         "Shift",         "Ctrl+Shift",
    "Alt",     "Ctrl+Alt",     "Shift+Alt",     "Ctrl+Shift+Alt",
    "GUI",     "Ctrl+GUI",     "Shift+GUI",     "Ctrl+Shift+GUI",
    "Alt+GUI", "Ctrl+Alt+GUI", "Shift+Alt+GUI", "Ctrl+Shift+Alt+GUI",
};

// #endregion

// #region Static functions

/**
 * Gives the name of the modifier set of a keycode.
 *
 * @param keycode Mod-tap or modified keycode.
 * @return Modifier names with `+` between them.
 */
static const char *modifier_set_name(uint16_t keycode) {
    return modifier_set_names[((unsigned)keycode >> HOLD_SHIFT) & MODIFIER_SET_MASK];
}

/**
 * Finds the own name of a keycode.
 *
 * @param keycode QMK keycode.
 * @return Name, or NULL for a keycode that named_keycodes does not have.
 */
static const char *find_own_name(uint16_t keycode) {
    for (size_t index = 0; index < COUNT_OF(named_keycodes); index++) {
        if (named_keycodes[index].keycode == keycode) {
            return named_keycodes[index].name;
        }
    }
    return NULL;
}

/**
 * Finds the name of a keycode that has no key and no layer in its bits.
 *
 * @param keycode QMK keycode.
 * @return Name, or NULL for a keycode with no name.
 */
static const char *find_plain_name(uint16_t keycode) {
    const char *own_name = find_own_name(keycode);
    if (own_name != NULL) {
        return own_name;
    }
    return usage_name(keycode);
}

/**
 * Writes the name of a keycode that has no key and no layer in its bits, or its
 * hexadecimal value.
 *
 * @param keycode QMK keycode.
 * @param name    Buffer that gets the name.
 * @param size    Bytes of the buffer.
 */
static void write_plain_name(uint16_t keycode, char *name, size_t size) {
    const char *plain_name = find_plain_name(keycode);
    if (plain_name == NULL) {
        snprintf(name, size, "0x%04X", (unsigned)keycode);
        return;
    }
    snprintf(name, size, "%s", plain_name);
}

/**
 * Writes the name of the tap key of a keycode.
 *
 * @param keycode Mod-tap, layer-tap or modified keycode.
 * @param tap_key Buffer of TAP_KEY_NAME_SIZE bytes that gets the name.
 */
static void write_tap_key_name(uint16_t keycode, char tap_key[TAP_KEY_NAME_SIZE]) {
    write_plain_name(keycode & TAP_KEY_MASK, tap_key, TAP_KEY_NAME_SIZE);
}

/**
 * Writes the name of a key with modifiers, for example `Shift+2`. Left and right
 * modifiers have the same name, as in a shortcut name.
 *
 * @param keycode Modified keycode.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
static void write_modified_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]) {
    char tap_key[TAP_KEY_NAME_SIZE];
    write_tap_key_name(keycode, tap_key);
    snprintf(name, KEYCODE_NAME_SIZE, "%s+%s", modifier_set_name(keycode), tap_key);
}

/**
 * Writes the name of a mod-tap key: its tap key, then the modifiers of its hold,
 * for example `T / Left GUI`.
 *
 * @param keycode Mod-tap keycode.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
static void write_mod_tap_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]) {
    char        tap_key[TAP_KEY_NAME_SIZE];
    const char *side = (keycode & RIGHT_MODIFIERS_BIT) == 0 ? "Left" : "Right";
    const char *hold = modifier_set_name(keycode);
    write_tap_key_name(keycode, tap_key);
    snprintf(name, KEYCODE_NAME_SIZE, "%s / %s %s", tap_key, side, hold);
}

/**
 * Writes the name of a layer-tap key: its tap key, then the layer of its hold, for
 * example `L / layer 0`.
 *
 * @param keycode Layer-tap keycode.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
static void write_layer_tap_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]) {
    unsigned layer = ((unsigned)keycode >> HOLD_SHIFT) & HOLD_LAYER_MASK;
    char     tap_key[TAP_KEY_NAME_SIZE];
    write_tap_key_name(keycode, tap_key);
    snprintf(name, KEYCODE_NAME_SIZE, "%s / layer %u", tap_key, layer);
}

/**
 * Writes the name of a layer toggle key, as the keymap writes it, for example
 * `TG(1)`.
 *
 * @param keycode Layer toggle keycode.
 * @param name    Buffer of KEYCODE_NAME_SIZE bytes that gets the name.
 */
static void write_toggle_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]) {
    snprintf(name, KEYCODE_NAME_SIZE, "TG(%u)", (unsigned)keycode & TOGGLE_LAYER_MASK);
}

/**
 * Finds the range of a keycode.
 *
 * @param keycode QMK keycode.
 * @return Range, or NULL for a keycode in no range.
 */
static const keycode_range_t *find_range(uint16_t keycode) {
    for (size_t index = 0; index < COUNT_OF(keycode_ranges); index++) {
        const keycode_range_t *range = &keycode_ranges[index];
        if (keycode >= range->first && keycode <= range->last) {
            return range;
        }
    }
    return NULL;
}

// #endregion

// #region Public functions

void write_keycode_name(uint16_t keycode, char name[KEYCODE_NAME_SIZE]) {
    const keycode_range_t *range = find_range(keycode);
    // A keycode with its own name can be in a range, for example the special mode key
    if (range == NULL || find_own_name(keycode) != NULL) {
        write_plain_name(keycode, name, KEYCODE_NAME_SIZE);
        return;
    }
    // mull-ignore-next: cxx_remove_void_call
    range->write(keycode, name);
}

// #endregion
