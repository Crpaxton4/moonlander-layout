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
    /** Ctrl+number and tab, browser and editor shortcuts. */
    LAYER_SHORTCUTS = 2,
    /** Mostly transparent gate to the text navigation layer. */
    LAYER_NAVIGATION_GATE = 3,
    /** Ctrl/Shift+arrows and word selection. */
    LAYER_TEXT_NAVIGATION = 4,
    /** Bootloader, NKRO, EEPROM clear, RGB and music keys. */
    LAYER_SYSTEM = 5,
    /** Every key disabled; double-hold either thumb dance to leave. */
    LAYER_LOCK = 6,
    /** Number of layers. */
    LAYER_COUNT,
};
