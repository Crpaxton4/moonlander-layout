/** @file
 * Lock layer lighting: only the two large thumb keys, which leave the layer, lit
 * white.
 */

// clang-format off
[LAYER_LOCK] = {
    // left half, outer column first
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF,
    // left thumb keys, then the large thumb key
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_WHITE,
    // right half, outer column first
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,   COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_OFF,
    COLOR_OFF, COLOR_OFF, COLOR_OFF,
    // right thumb keys, then the large thumb key
    COLOR_OFF, COLOR_OFF, COLOR_OFF, COLOR_WHITE
},
    // clang-format on
