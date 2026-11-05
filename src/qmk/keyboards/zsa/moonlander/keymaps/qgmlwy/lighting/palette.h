/** @file
 * Named key colours of the qgmlwy keymap's per-layer lighting.
 *
 * Each colour is an `{ hue, saturation, value }` initializer for one `ledmap`
 * entry, every component 0 to 255. The value is scaled by the RGB matrix
 * brightness when the colour is shown. Colours that differ by a step or two look
 * the same on the keyboard; the hue in their name tells them apart.
 */

#pragma once

/** Off: `set_layer_color()` turns an all-zero entry's LED off. */
#define COLOR_OFF {0, 0, 0}
/** White at under half brightness. */
#define COLOR_GREY {0, 0, 112}
/** White. */
#define COLOR_WHITE {0, 0, 255}
/** White with a faint blue tint. */
#define COLOR_COOL_WHITE {139, 16, 255}
/** Dark red. */
#define COLOR_DARK_RED {0, 255, 163}
/** Red. */
#define COLOR_RED {0, 255, 228}
/** Red with a touch of orange. */
#define COLOR_SCARLET {5, 255, 255}
/** Orange. */
#define COLOR_ORANGE {18, 218, 204}
/** Yellow green. */
#define COLOR_YELLOW_GREEN {52, 235, 255}
/** Chartreuse, a green yellow. */
#define COLOR_CHARTREUSE {66, 234, 255}
/** Green, hue 92. */
#define COLOR_GREEN_92 {92, 255, 255}
/** Green, hue 95. */
#define COLOR_GREEN_95 {95, 255, 255}
/** Green, hue 96. */
#define COLOR_GREEN_96 {96, 255, 255}
/** Green, hue 97. */
#define COLOR_GREEN_97 {97, 255, 255}
/** Aquamarine, between green and cyan. */
#define COLOR_AQUAMARINE {116, 255, 255}
/** Cyan. */
#define COLOR_CYAN {129, 255, 255}
/** Sky blue. */
#define COLOR_SKY_BLUE {139, 255, 239}
/** Pale greyish blue. */
#define COLOR_PALE_BLUE {139, 33, 224}
/** Blue, hue 179. */
#define COLOR_BLUE_179 {179, 255, 255}
/** Blue, hue 180. */
#define COLOR_BLUE_180 {180, 255, 255}
/** Purple, hue 186. */
#define COLOR_PURPLE_186 {186, 197, 255}
/** Purple, hue 187, a little dimmer. */
#define COLOR_PURPLE_187 {187, 195, 250}
/** Lavender, a pale purple. */
#define COLOR_LAVENDER {187, 129, 237}
/** Violet, a deep blue purple. */
#define COLOR_VIOLET {187, 252, 255}
/** Pink. */
#define COLOR_PINK {237, 255, 255}
