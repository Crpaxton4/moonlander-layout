/** @file
 * Text navigation layer: Ctrl, Alt and Shift arrow chords on the right half for
 * moving by word and selecting text. The escape position goes back to the
 * navigation gate layer.
 */

// clang-format off
[LAYER_TEXT_NAVIGATION] = LAYOUT_moonlander(
    TO(LAYER_NAVIGATION_GATE), _______, _______, _______,                   _______,    _______, _______,        _______,        _______, LCTL(KC_1),          LCTL(KC_2),        LCTL(KC_3),          LCTL(KC_4),           LCTL(KC_X),
    _______,                   _______, _______, _______,                   _______,    _______, _______,        TO(LAYER_BASE), _______, LCTL(KC_LEFT),       LCTL(KC_UP),       LCTL(KC_DOWN),       LCTL(KC_RIGHT),       _______,
    _______,                   _______, _______, _______,                   LCTL(KC_S), _______, _______,        _______,        _______, LALT(LSFT(KC_LEFT)), LCTL(LSFT(KC_UP)), LCTL(LSFT(KC_DOWN)), LALT(LSFT(KC_RIGHT)), _______,
    _______,                   _______, _______, _______,                   _______,    _______,                                 _______, LSFT(KC_LEFT),       LSFT(KC_UP),       LSFT(KC_DOWN),       LSFT(KC_RIGHT),       _______,
    _______,                   _______, _______, TO(LAYER_NAVIGATION_GATE), _______,    _______,                                 _______, _______,             _______,           _______,             _______,              _______,
                                                                            _______,    _______, _______,        _______,        _______, _______
),
    // clang-format on
