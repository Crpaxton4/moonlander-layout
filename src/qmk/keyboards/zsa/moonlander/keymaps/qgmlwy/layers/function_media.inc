/** @file
 * Function and media layer: F1 to F20 across the top two rows, plus media
 * playback and volume keys on the right half.
 */

// clang-format off
[LAYER_FUNCTION_MEDIA] = LAYOUT_moonlander(
    TO(LAYER_BASE),    _______, KC_F1,          KC_F2,      KC_F3,        KC_F4,   KC_F5,                      KC_F6,   KC_F7,   KC_F8,         KC_F9,               KC_F10,            _______,     _______,
    TD(DANCE_6),       _______, KC_F11,         KC_F12,     KC_F13,       KC_F14,  KC_F15,                     KC_F16,  KC_F17,  KC_F18,        KC_F19,              KC_F20,            _______,     _______,
    KC_LEFT_GUI,       _______, _______,        LGUI(KC_E), LGUI(KC_DOT), _______, LALT(LGUI(KC_LBRC)),        _______, _______, DUAL_FUNC_2,   KC_MEDIA_PLAY_PAUSE, KC_MEDIA_STOP,     DUAL_FUNC_3, _______,
    _______,           _______, KC_F13,         _______,    _______,      _______,                                      _______, KC_AUDIO_MUTE, KC_AUDIO_VOL_UP,     KC_AUDIO_VOL_DOWN, _______,     _______,
    _______,           _______, TO(LAYER_BASE), _______,    _______,      _______,                                      _______, _______,       _______,             _______,           _______,     _______,
                                                            _______,      _______, _______,                    _______, _______, _______
),
    // clang-format on
