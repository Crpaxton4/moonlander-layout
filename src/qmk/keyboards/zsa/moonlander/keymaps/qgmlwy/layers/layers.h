/** @file
 * Layer names of the qgmlwy keymap, and names for the rows and keys of a layer.
 *
 * `keymaps` gives each row of a layer as `[layer][row] = {keys}`, so the index
 * names the place of each key and a row with too many keys does not compile.
 */

#pragma once

/** Keymap layers, in the order of their index in `keymaps` */
enum layer {
    LAYER_BASE   = 0, /**< QGMLWY alphas with mouse keys bottom right */
    LAYER_ARROWS = 1, /**< Base layer with arrows in place of mouse movement */
};

/**
 * Rows of a layer, each the index of a row in the Moonlander key matrix.
 *
 * The matrix has the left half in rows 0 to 5 and the right half in rows 6 to 11,
 * and each half keeps its thumb keys in its last row. Each row comment names the
 * grid row and its keys, in matrix order from left to right.
 */
enum layer_row {
    LEFT_NUMBER_ROW         = 0,  /**< Row 0: (0,0) to (0,6) */
    LEFT_TOP_ROW            = 1,  /**< Row 1: (1,0) to (1,6) */
    LEFT_HOME_ROW           = 2,  /**< Row 2: (2,0) to (2,6) */
    LEFT_BOTTOM_LETTER_ROW  = 3,  /**< Row 3: (3,0) to (3,5), then NO_PHYSICAL_KEY */
    LEFT_BOTTOM_ROW         = 4,  /**< Row 4: (4,0) to (4,4), two NO_PHYSICAL_KEY */
    LEFT_THUMB_KEYS         = 5,  /**< Thumb keys L0 to L3, indexed by `thumb_key` */
    RIGHT_NUMBER_ROW        = 6,  /**< Row 0: (0,7) to (0,13) */
    RIGHT_TOP_ROW           = 7,  /**< Row 1: (1,7) to (1,13) */
    RIGHT_HOME_ROW          = 8,  /**< Row 2: (2,7) to (2,13) */
    RIGHT_BOTTOM_LETTER_ROW = 9,  /**< Row 3: NO_PHYSICAL_KEY, then (3,8) to (3,13) */
    RIGHT_BOTTOM_ROW        = 10, /**< Row 4: two NO_PHYSICAL_KEY, (4,9) to (4,13) */
    RIGHT_THUMB_KEYS        = 11, /**< Thumb keys R0 to R3, indexed by `thumb_key` */
};

/**
 * Matrix position with no physical key, which keeps the other keys of the row in
 * their matrix columns.
 */
#define NO_PHYSICAL_KEY KC_NO

/** Thumb keys in LEFT_THUMB_KEYS and RIGHT_THUMB_KEYS */
enum thumb_key {
    LEFT_LARGE_THUMB   = 3, /**< L0, large left thumb key */
    LEFT_OUTER_THUMB   = 0, /**< L1, small left thumb key farthest from the middle */
    LEFT_MIDDLE_THUMB  = 1, /**< L2, small left thumb key between the other two */
    LEFT_INNER_THUMB   = 2, /**< L3, small left thumb key nearest to the middle */
    RIGHT_LARGE_THUMB  = 3, /**< R0, large right thumb key */
    RIGHT_OUTER_THUMB  = 6, /**< R1, small right thumb key farthest from the middle */
    RIGHT_MIDDLE_THUMB = 5, /**< R2, small right thumb key between the other two */
    RIGHT_INNER_THUMB  = 4, /**< R3, small right thumb key nearest to the middle */
};
