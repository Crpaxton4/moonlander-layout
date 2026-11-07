/** @file
 * Usage table of the HID keyboard page for the Windows US English layout
 */

// #region Includes

#include <stdio.h>
#include <string.h>

#include "hid_usage.h"
#include "support.h"

// #endregion

// #region Types

/** Name and characters of one HID usage */
typedef struct {
    const char *name;    /**< Key name in shortcut names */
    char        plain;   /**< Character with no Shift, or '\0' */
    char        shifted; /**< Character with Shift, or '\0' */
} usage_t;

/** Modifier of a shortcut name and its HID modifier bits */
typedef struct {
    uint8_t     mask; /**< Left and right bits */
    const char *name; /**< Name in the shortcut */
} modifier_t;

// #endregion

// #region Data

/**
 * Usages, indexed by HID usage. The keypad assumes Num Lock on. With Shift, Windows
 * then makes a keypad digit a cursor key, which types no character.
 */
static const usage_t usages[] = {
    [0x04] = {"A", 'a', 'A'},
    [0x05] = {"B", 'b', 'B'},
    [0x06] = {"C", 'c', 'C'},
    [0x07] = {"D", 'd', 'D'},
    [0x08] = {"E", 'e', 'E'},
    [0x09] = {"F", 'f', 'F'},
    [0x0A] = {"G", 'g', 'G'},
    [0x0B] = {"H", 'h', 'H'},
    [0x0C] = {"I", 'i', 'I'},
    [0x0D] = {"J", 'j', 'J'},
    [0x0E] = {"K", 'k', 'K'},
    [0x0F] = {"L", 'l', 'L'},
    [0x10] = {"M", 'm', 'M'},
    [0x11] = {"N", 'n', 'N'},
    [0x12] = {"O", 'o', 'O'},
    [0x13] = {"P", 'p', 'P'},
    [0x14] = {"Q", 'q', 'Q'},
    [0x15] = {"R", 'r', 'R'},
    [0x16] = {"S", 's', 'S'},
    [0x17] = {"T", 't', 'T'},
    [0x18] = {"U", 'u', 'U'},
    [0x19] = {"V", 'v', 'V'},
    [0x1A] = {"W", 'w', 'W'},
    [0x1B] = {"X", 'x', 'X'},
    [0x1C] = {"Y", 'y', 'Y'},
    [0x1D] = {"Z", 'z', 'Z'},
    [0x1E] = {"1", '1', '!'},
    [0x1F] = {"2", '2', '@'},
    [0x20] = {"3", '3', '#'},
    [0x21] = {"4", '4', '$'},
    [0x22] = {"5", '5', '%'},
    [0x23] = {"6", '6', '^'},
    [0x24] = {"7", '7', '&'},
    [0x25] = {"8", '8', '*'},
    [0x26] = {"9", '9', '('},
    [0x27] = {"0", '0', ')'},
    [0x28] = {"Enter", '\n', '\n'},
    [0x29] = {"Escape", '\0', '\0'},
    [0x2A] = {"Backspace", '\0', '\0'},
    [0x2B] = {"Tab", '\t', '\0'},
    [0x2C] = {"Space", ' ', ' '},
    [0x2D] = {"Minus", '-', '_'},
    [0x2E] = {"Equal", '=', '+'},
    [0x2F] = {"LeftBracket", '[', '{'},
    [0x30] = {"RightBracket", ']', '}'},
    [0x31] = {"Backslash", '\\', '|'},
    [0x32] = {"NonUsHash", '\\', '|'},
    [0x33] = {"Semicolon", ';', ':'},
    [0x34] = {"Quote", '\'', '"'},
    [0x35] = {"Grave", '`', '~'},
    [0x36] = {"Comma", ',', '<'},
    [0x37] = {"Dot", '.', '>'},
    [0x38] = {"Slash", '/', '?'},
    [0x39] = {"CapsLock", '\0', '\0'},
    [0x3A] = {"F1", '\0', '\0'},
    [0x3B] = {"F2", '\0', '\0'},
    [0x3C] = {"F3", '\0', '\0'},
    [0x3D] = {"F4", '\0', '\0'},
    [0x3E] = {"F5", '\0', '\0'},
    [0x3F] = {"F6", '\0', '\0'},
    [0x40] = {"F7", '\0', '\0'},
    [0x41] = {"F8", '\0', '\0'},
    [0x42] = {"F9", '\0', '\0'},
    [0x43] = {"F10", '\0', '\0'},
    [0x44] = {"F11", '\0', '\0'},
    [0x45] = {"F12", '\0', '\0'},
    [0x46] = {"PrintScreen", '\0', '\0'},
    [0x47] = {"ScrollLock", '\0', '\0'},
    [0x48] = {"Pause", '\0', '\0'},
    [0x49] = {"Insert", '\0', '\0'},
    [0x4A] = {"Home", '\0', '\0'},
    [0x4B] = {"PageUp", '\0', '\0'},
    [0x4C] = {"Delete", '\0', '\0'},
    [0x4D] = {"End", '\0', '\0'},
    [0x4E] = {"PageDown", '\0', '\0'},
    [0x4F] = {"Right", '\0', '\0'},
    [0x50] = {"Left", '\0', '\0'},
    [0x51] = {"Down", '\0', '\0'},
    [0x52] = {"Up", '\0', '\0'},
    [0x53] = {"NumLock", '\0', '\0'},
    [0x54] = {"KeypadSlash", '/', '/'},
    [0x55] = {"KeypadAsterisk", '*', '*'},
    [0x56] = {"KeypadMinus", '-', '-'},
    [0x57] = {"KeypadPlus", '+', '+'},
    [0x58] = {"KeypadEnter", '\n', '\n'},
    [0x59] = {"Keypad1", '1', '\0'},
    [0x5A] = {"Keypad2", '2', '\0'},
    [0x5B] = {"Keypad3", '3', '\0'},
    [0x5C] = {"Keypad4", '4', '\0'},
    [0x5D] = {"Keypad5", '5', '\0'},
    [0x5E] = {"Keypad6", '6', '\0'},
    [0x5F] = {"Keypad7", '7', '\0'},
    [0x60] = {"Keypad8", '8', '\0'},
    [0x61] = {"Keypad9", '9', '\0'},
    [0x62] = {"Keypad0", '0', '\0'},
    [0x63] = {"KeypadDot", '.', '\0'},
    [0x64] = {"NonUsBackslash", '\\', '|'},
    [0x65] = {"Application", '\0', '\0'},
    [0x66] = {"Power", '\0', '\0'},
    [0x67] = {"KeypadEqual", '\0', '\0'},
    [0x68] = {"F13", '\0', '\0'},
    [0x69] = {"F14", '\0', '\0'},
    [0x6A] = {"F15", '\0', '\0'},
    [0x6B] = {"F16", '\0', '\0'},
    [0x6C] = {"F17", '\0', '\0'},
    [0x6D] = {"F18", '\0', '\0'},
    [0x6E] = {"F19", '\0', '\0'},
    [0x6F] = {"F20", '\0', '\0'},
    [0x70] = {"F21", '\0', '\0'},
    [0x71] = {"F22", '\0', '\0'},
    [0x72] = {"F23", '\0', '\0'},
    [0x73] = {"F24", '\0', '\0'},
};

/** Modifiers in the order of the shortcut names */
static const modifier_t modifiers[] = {
    {MODS_CTRL, "Ctrl+"},
    {MODS_SHIFT, "Shift+"},
    {MODS_ALT, "Alt+"},
    {MODS_GUI, "GUI+"},
};

// #endregion

// #region Static functions

/**
 * Finds the table entry of a usage.
 *
 * @param usage HID usage.
 * @return Entry, or NULL for a usage that the table does not have.
 */
static const usage_t *find_usage(uint8_t usage) {
    if (usage >= COUNT_OF(usages) || usages[usage].name == NULL) {
        return NULL;
    }
    return &usages[usage];
}

/**
 * Adds text to the end of a shortcut name. snprintf() cuts a text that is too long.
 *
 * @param name Buffer of SHORTCUT_NAME_SIZE bytes with a name.
 * @param text Text to add.
 */
static void append_name(char name[SHORTCUT_NAME_SIZE], const char *text) {
    size_t length = strlen(name);
    snprintf(name + length, SHORTCUT_NAME_SIZE - length, "%s", text);
}

// #endregion

// #region Public functions

char usage_character(uint8_t usage, bool shifted) {
    const usage_t *entry = find_usage(usage);
    if (entry == NULL) {
        return '\0';
    }
    return shifted ? entry->shifted : entry->plain;
}

void write_shortcut_name(key_press_t press, char name[SHORTCUT_NAME_SIZE]) {
    name[0] = '\0';
    for (size_t index = 0; index < COUNT_OF(modifiers); index++) {
        if ((press.mods & modifiers[index].mask) != 0) {
            append_name(name, modifiers[index].name);
        }
    }
    const usage_t *entry = find_usage(press.usage);
    if (entry == NULL) {
        char code[sizeof("0xFF")];
        snprintf(code, sizeof(code), "0x%02X", (unsigned)press.usage);
        append_name(name, code);
        return;
    }
    append_name(name, entry->name);
}

// #endregion
