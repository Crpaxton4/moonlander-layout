/** @file
 * Base layer lighting: the shortcuts one-shot key scarlet, mouse movement and the
 * right thumb keys blue, and the right large thumb key, which switches to the
 * arrows layer, orange.
 */

// clang-format off
[LAYER_BASE] = {
    // left half, outer column first
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,
    // left thumb keys, then the large thumb key
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    // right half, outer column first
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_BLUE_180,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_BLUE_180,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_BLUE_180,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,    COLOR_BLUE_179,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_COOL_WHITE, COLOR_SCARLET,
    // right thumb keys, then the large thumb key
    COLOR_BLUE_180, COLOR_BLUE_180,   COLOR_BLUE_180, COLOR_ORANGE
},
    // clang-format on
