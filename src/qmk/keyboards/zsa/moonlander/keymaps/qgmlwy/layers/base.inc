/** @file
 * Base layer: QGMLWY alphas and the number row, and mouse movement and buttons on
 * the bottom right.
 */

// clang-format off
[LAYER_BASE] = LAYOUT_moonlander(
    KC_ESCAPE,     KC_GRAVE,    KC_1,                      KC_2,                KC_3,                      KC_4,     KC_5,               KC_6,                 KC_7,             KC_8,       KC_9,     KC_0,       KC_MINUS,    KC_EQUAL,
    KC_TAB,        KC_Q,        KC_G,                      KC_M,                KC_L,                      KC_W,     DUAL_FUNC_0,        TD(DANCE_1),          KC_Y,             KC_F,       KC_U,     KC_B,       KC_SCLN,     KC_BSLS,
    LSFT(KC_TAB),  KC_D,        KC_S,                      KC_T,                KC_N,                      KC_R,     DUAL_FUNC_1,        KC_NO,                KC_I,             KC_A,       KC_E,     KC_O,       KC_H,        KC_QUOTE,
    KC_LEFT_SHIFT, KC_Z,        KC_X,                      KC_C,                KC_V,                      KC_J,                                               KC_K,             KC_P,       KC_COMMA, KC_DOT,     KC_SLASH,    TD(DANCE_2),
    TD(DANCE_0),   KC_LEFT_ALT, KC_NO,                     KC_NO,               KC_NO,                     KC_ENTER,                                           TO(LAYER_ARROWS), KC_MS_LEFT, KC_MS_UP, KC_MS_DOWN, KC_MS_RIGHT, TD(DANCE_3),
                                                                                KC_SPACE,                  KC_BSPC,  KC_DELETE,          TD(DANCE_4),          TD(DANCE_5),      KC_MS_BTN1
),
    // clang-format on
