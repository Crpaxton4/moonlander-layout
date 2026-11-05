/** @file
 * Function and media layer lighting: F keys green, media and volume keys
 * aquamarine, the top left key back to the base layer white, a few single keys
 * in colours of their own, and the remaining lights as on the base layer.
 */

// clang-format off
[LAYER_FUNCTION_MEDIA] = {
    // left half, outer column first
    COLOR_WHITE,    COLOR_RED,      COLOR_GREY,         COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,      COLOR_OFF,          COLOR_OFF,        COLOR_OFF,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_OFF,          COLOR_CYAN,       COLOR_GREEN_92,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_SKY_BLUE,     COLOR_OFF,        COLOR_PINK,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_YELLOW_GREEN, COLOR_OFF,        COLOR_PURPLE_186,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_OFF,          COLOR_OFF,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_AQUAMARINE,
    // left thumb keys, then the large thumb key
    COLOR_OFF,      COLOR_OFF,      COLOR_OFF,          COLOR_OFF,
    // right half, outer column first
    COLOR_OFF,      COLOR_OFF,      COLOR_OFF,          COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,      COLOR_AQUAMARINE,   COLOR_OFF,        COLOR_BLUE_180,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_AQUAMARINE,   COLOR_AQUAMARINE, COLOR_BLUE_180,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_AQUAMARINE,   COLOR_AQUAMARINE, COLOR_BLUE_180,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_AQUAMARINE,   COLOR_AQUAMARINE, COLOR_BLUE_180,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_OFF,          COLOR_OFF,
    COLOR_GREEN_92, COLOR_GREEN_92, COLOR_SCARLET,
    // right thumb keys, then the large thumb key
    COLOR_BLUE_180, COLOR_BLUE_180, COLOR_BLUE_180,     COLOR_ORANGE
},
    // clang-format on
