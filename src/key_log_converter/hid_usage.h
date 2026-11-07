/** @file
 * HID keyboard usages: characters of the Windows US English layout, and key names
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** HID usage of Backspace, also the QMK keycode KC_BACKSPACE */
#define USAGE_BACKSPACE 0x2A

/** HID usage of Delete forward */
#define USAGE_DELETE 0x4C

/** Left and right Ctrl bits of the HID modifier byte */
#define MODS_CTRL 0x11

/** Left and right Shift bits of the HID modifier byte */
#define MODS_SHIFT 0x22

/** Left and right Alt bits of the HID modifier byte */
#define MODS_ALT 0x44

/** Left and right GUI bits of the HID modifier byte */
#define MODS_GUI 0x88

/** Modifiers that make a key push a shortcut, not text */
#define MODS_SHORTCUT (MODS_CTRL | MODS_ALT | MODS_GUI)

/** Bytes of the longest shortcut name, with its terminator */
#define SHORTCUT_NAME_SIZE 48

/** New key of a report, with the modifiers of that report */
typedef struct {
    uint8_t mods;  /**< HID modifier byte */
    uint8_t usage; /**< HID usage of the key */
} key_press_t;

/**
 * Gives the character that a key types on the Windows US English layout.
 *
 * @param usage   HID usage of the key.
 * @param shifted true while Shift is down.
 * @return Character, or '\0' for a key that types no character.
 */
char usage_character(uint8_t usage, bool shifted);

/**
 * Writes the name of a shortcut, for example `Ctrl+Shift+T`.
 *
 * @param press Key and modifiers.
 * @param name  Buffer of SHORTCUT_NAME_SIZE bytes that gets the name.
 */
void write_shortcut_name(key_press_t press, char name[SHORTCUT_NAME_SIZE]);
