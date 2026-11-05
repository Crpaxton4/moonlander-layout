/** @file
 * Layer names of the qgmlwy keymap.
 *
 * Each layer's keys live in `layers/<name>.inc`, laid out like the keyboard: left
 * and right half side by side, one row per line, the large thumb key at the inner
 * end of the fifth row and the three thumb keys on the last line.
 */

#pragma once

/**
 * Keymap layers, in the order of their index in `keymaps` and `ledmap`.
 */
enum layer {
    /** QGMLWY alphas with mouse keys bottom right. */
    LAYER_BASE = 0,
    /** Base layer with arrows in place of mouse movement. */
    LAYER_ARROWS = 1,
    /** F1 to F20, media and volume keys. */
    LAYER_FUNCTION_MEDIA = 2,
    /** Ctrl+number and tab, browser and editor shortcuts. */
    LAYER_SHORTCUTS = 3,
    /** Alt+Ctrl+Shift+grave leader chord macros. */
    LAYER_LAUNCHER = 4,
    /** Ctrl/Alt/Shift+F13 to F18 bindings and window keys. */
    LAYER_WINDOW_MANAGER = 5,
    /** Mostly transparent gate to the text navigation layer. */
    LAYER_NAVIGATION_GATE = 6,
    /** Ctrl/Shift+arrows and word selection. */
    LAYER_TEXT_NAVIGATION = 7,
    /** Bootloader, NKRO, EEPROM clear, RGB and music keys. */
    LAYER_SYSTEM = 8,
    /** Every key disabled; double-hold either thumb dance to leave. */
    LAYER_LOCK = 9,
    /** Number of layers. */
    LAYER_COUNT,
};
