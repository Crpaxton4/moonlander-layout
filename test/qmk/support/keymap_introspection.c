/** @file
 * Host version of QMK's keymap introspection and keymap lookups: the tests read the
 * keymap tables through QMK's own functions.
 *
 * QMK compiles keymap.c only inside its introspection file, which sizes the tables
 * with `sizeof`. Thus, the tests compile keymap.c in the same way, and read the
 * tables as the firmware does.
 */

// #region Includes

// clang-tidy reports each include of a .c file, but QMK uses the same include
// NOLINTNEXTLINE(bugprone-suspicious-include)
#include KEYMAP_C

#include "keymap_introspection.h"

// #endregion

// #region Macros

/** Number of layers in `keymaps` */
#define KEYMAP_LAYER_COUNT (sizeof(keymaps) / sizeof(keymaps[0]))

/** Lowest bit of an unsigned value */
#define LOWEST_BIT 1U

// #endregion

// #region Public functions

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

uint16_t combo_count(void) {
    return (uint16_t)ARRAY_SIZE(key_combos);
}

uint16_t keymap_key_to_keycode(uint8_t layer, keypos_t key) {
    // Same result as QMK for a position outside the matrix
    if (key.row >= MATRIX_ROWS || key.col >= MATRIX_COLS) {
        return KC_NO;
    }
    return keycode_at_keymap_location(layer, key.row, key.col);
}

uint8_t layer_switch_get_layer(keypos_t key) {
    // Same result as QMK: the highest active layer with no transparent key at the
    // position, else layer 0
    layer_state_t active_layers = layer_state | default_layer_state;
    uint8_t       found_layer   = 0;
    for (uint8_t layer = 0; layer < MAX_LAYER; layer++) {
        bool is_active = (active_layers & ((layer_state_t)LOWEST_BIT << layer)) != 0;
        if (is_active && keymap_key_to_keycode(layer, key) != KC_TRANSPARENT) {
            found_layer = layer;
        }
    }
    return found_layer;
}

combo_t *combo_get(uint16_t combo_index) {
    // Same result as QMK for an index outside the table
    if (combo_index >= combo_count()) {
        return NULL;
    }
    return &key_combos[combo_index];
}

// #endregion
