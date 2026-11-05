/** @file
 * System layer lighting: the bootloader key dark red, NKRO and EEPROM clear
 * scarlet, the RGB, music and lock keys green, the keys back to the base layer
 * white, and the remaining lights as on the base layer.
 */

// clang-format off
[LAYER_SYSTEM] = {
    // left half, outer column first
    COLOR_WHITE,    COLOR_DARK_RED,   COLOR_OFF,      COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_SCARLET,    COLOR_GREEN_95, COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_SCARLET,    COLOR_GREEN_96, COLOR_OFF,      COLOR_GREEN_92,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,      COLOR_PINK,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,      COLOR_PURPLE_186,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,
    // left thumb keys, then the large thumb key
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,
    // right half, outer column first
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,      COLOR_OFF,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,      COLOR_BLUE_179,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_OFF,      COLOR_BLUE_179,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_GREEN_96, COLOR_BLUE_179,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_GREEN_97, COLOR_BLUE_179,
    COLOR_OFF,      COLOR_OFF,        COLOR_OFF,      COLOR_GREEN_97,
    COLOR_OFF,      COLOR_COOL_WHITE, COLOR_SCARLET,
    // right thumb keys, then the large thumb key
    COLOR_BLUE_179, COLOR_BLUE_179,   COLOR_BLUE_179, COLOR_ORANGE
},
    // clang-format on
