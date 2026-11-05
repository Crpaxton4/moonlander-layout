/** @file
 * Text navigation layer lighting: the movement, selection and other editing keys
 * chartreuse, the top left key back to the navigation gate white, and the
 * remaining lights as on the base layer.
 */

// clang-format off
[LAYER_TEXT_NAVIGATION] = {
    // left half, outer column first
    COLOR_WHITE,      COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_GREEN_92,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_CHARTREUSE,
    COLOR_OFF,        COLOR_OFF,        COLOR_CHARTREUSE, COLOR_OFF,        COLOR_PURPLE_186,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    // left thumb keys, then the large thumb key
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    // right half, outer column first
    COLOR_CHARTREUSE, COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_BLUE_179,
    COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_BLUE_179,
    COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_BLUE_179,
    COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_CHARTREUSE, COLOR_BLUE_179,
    COLOR_OFF,        COLOR_OFF,        COLOR_OFF,        COLOR_OFF,
    COLOR_OFF,        COLOR_BLUE_179,   COLOR_SCARLET,
    // right thumb keys, then the large thumb key
    COLOR_BLUE_179,   COLOR_BLUE_179,   COLOR_BLUE_179,   COLOR_ORANGE
},
    // clang-format on
