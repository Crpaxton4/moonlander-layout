/** @file
 * Layer names of the qgmlwy keymap, and names for the rows and keys of a layer.
 *
 * `keymaps` gives each row of a layer as `[layer][row] = {keys}`, so the index
 * names the place of each key and a row with too many keys does not compile.
 */

#pragma once

/** Keymap layers, in the order of their index in `keymaps` */
enum layer {
    /** QGMLWY alphas with mouse keys bottom right. */
    LAYER_BASE = 0,
    /** Base layer with arrows in place of mouse movement. */
    LAYER_ARROWS = 1,
};

/**
 * Rows of a layer, each the index of a row in the Moonlander key matrix.
 *
 * The matrix has the left half in rows 0 to 5 and the right half in rows 6 to 11,
 * and each half keeps its thumb keys in its last row. A grid row lists its seven
 * matrix positions from left to right.
 */
enum layer_row {
    /** Grid row 0, keys (0,0) to (0,6) */
    LEFT_NUMBER_ROW = 0,
    /** Grid row 1, keys (1,0) to (1,6) */
    LEFT_TOP_ROW = 1,
    /** Grid row 2, keys (2,0) to (2,6) */
    LEFT_HOME_ROW = 2,
    /** Grid row 3, keys (3,0) to (3,5), then NO_PHYSICAL_KEY */
    LEFT_BOTTOM_LETTER_ROW = 3,
    /** Grid row 4, keys (4,0) to (4,4), then two NO_PHYSICAL_KEY */
    LEFT_BOTTOM_ROW = 4,
    /** Thumb keys L0 to L3, by their `thumb_key` names */
    LEFT_THUMB_KEYS = 5,
    /** Grid row 0, keys (0,7) to (0,13) */
    RIGHT_NUMBER_ROW = 6,
    /** Grid row 1, keys (1,7) to (1,13) */
    RIGHT_TOP_ROW = 7,
    /** Grid row 2, keys (2,7) to (2,13) */
    RIGHT_HOME_ROW = 8,
    /** Grid row 3, NO_PHYSICAL_KEY, then keys (3,8) to (3,13) */
    RIGHT_BOTTOM_LETTER_ROW = 9,
    /** Grid row 4, two NO_PHYSICAL_KEY, then keys (4,9) to (4,13) */
    RIGHT_BOTTOM_ROW = 10,
    /** Thumb keys R0 to R3, by their `thumb_key` names */
    RIGHT_THUMB_KEYS = 11,
};

/**
 * Matrix position with no physical key, which keeps the other keys of the row in
 * their matrix columns.
 */
#define NO_PHYSICAL_KEY KC_NO

/** Thumb keys in LEFT_THUMB_KEYS and RIGHT_THUMB_KEYS */
enum thumb_key {
    /** L0, the large left thumb key */
    LEFT_LARGE_THUMB = 3,
    /** L1, the small left thumb key farthest from the middle of the keyboard */
    LEFT_OUTER_THUMB = 0,
    /** L2, the small left thumb key between the other two */
    LEFT_MIDDLE_THUMB = 1,
    /** L3, the small left thumb key nearest to the middle of the keyboard */
    LEFT_INNER_THUMB = 2,
    /** R0, the large right thumb key */
    RIGHT_LARGE_THUMB = 3,
    /** R1, the small right thumb key farthest from the middle of the keyboard */
    RIGHT_OUTER_THUMB = 6,
    /** R2, the small right thumb key between the other two */
    RIGHT_MIDDLE_THUMB = 5,
    /** R3, the small right thumb key nearest to the middle of the keyboard */
    RIGHT_INNER_THUMB = 4,
};
