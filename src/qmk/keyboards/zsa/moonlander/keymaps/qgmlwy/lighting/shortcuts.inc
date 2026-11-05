/** @file
 * Shortcuts layer lighting: every shortcut key scarlet, the top left key back to
 * the base layer white, and the remaining lights as on the base layer.
 */

// clang-format off
[LAYER_SHORTCUTS] = {
    // left half, outer column first
    COLOR_WHITE,    COLOR_OFF,        COLOR_OFF,      COLOR_OFF,     COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,     COLOR_OFF,
    COLOR_SCARLET,  COLOR_OFF,        COLOR_OFF,      COLOR_OFF,     COLOR_GREEN_92,
    COLOR_SCARLET,  COLOR_OFF,        COLOR_SCARLET,  COLOR_OFF,     COLOR_PINK,
    COLOR_SCARLET,  COLOR_OFF,        COLOR_SCARLET,  COLOR_OFF,     COLOR_PURPLE_186,
    COLOR_SCARLET,  COLOR_OFF,        COLOR_OFF,      COLOR_SCARLET,
    COLOR_SCARLET,  COLOR_SCARLET,    COLOR_SCARLET,
    // left thumb keys, then the large thumb key
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    // right half, outer column first
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_SCARLET, COLOR_SCARLET,
    COLOR_OFF,      COLOR_SCARLET,    COLOR_SCARLET,  COLOR_SCARLET, COLOR_BLUE_179,
    COLOR_OFF,      COLOR_SCARLET,    COLOR_SCARLET,  COLOR_SCARLET, COLOR_BLUE_179,
    COLOR_SCARLET,  COLOR_SCARLET,    COLOR_SCARLET,  COLOR_OFF,     COLOR_BLUE_179,
    COLOR_SCARLET,  COLOR_SCARLET,    COLOR_SCARLET,  COLOR_SCARLET, COLOR_BLUE_179,
    COLOR_SCARLET,  COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    COLOR_SCARLET,  COLOR_COOL_WHITE, COLOR_SCARLET,
    // right thumb keys, then the large thumb key
    COLOR_BLUE_179, COLOR_BLUE_179,   COLOR_BLUE_179, COLOR_ORANGE
},
    // clang-format on
