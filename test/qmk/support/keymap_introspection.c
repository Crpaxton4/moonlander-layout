/** @file
 * Host version of QMK's keymap introspection: the tests read the keymap tables
 * through QMK's own introspection functions.
 *
 * QMK compiles keymap.c only inside its introspection file, which sizes the tables
 * with `sizeof`. Thus, the tests compile keymap.c in the same way, and read the
 * tables as the firmware does.
 */

// clang-tidy reports each include of a .c file, but QMK uses the same include
// NOLINTNEXTLINE(bugprone-suspicious-include)
#include KEYMAP_C

#include "keymap_introspection.h"

/** Number of layers in `keymaps` */
#define KEYMAP_LAYER_COUNT (sizeof(keymaps) / sizeof(keymaps[0]))

uint8_t keymap_layer_count(void) {
    return (uint8_t)KEYMAP_LAYER_COUNT;
}

uint16_t keycode_at_keymap_location(uint8_t layer, uint8_t row, uint8_t column) {
    // Same result as QMK for a position outside the tables
    if (layer >= KEYMAP_LAYER_COUNT || row >= MATRIX_ROWS || column >= MATRIX_COLS) {
        return KC_TRANSPARENT;
    }
    return keymaps[layer][row][column];
}

uint16_t tap_dance_count(void) {
    return (uint16_t)ARRAY_SIZE(tap_dance_actions);
}

tap_dance_action_t *tap_dance_get(uint16_t slot) {
    // Same result as QMK for a slot outside the table
    if (slot >= tap_dance_count()) {
        return NULL;
    }
    return &tap_dance_actions[slot];
}
