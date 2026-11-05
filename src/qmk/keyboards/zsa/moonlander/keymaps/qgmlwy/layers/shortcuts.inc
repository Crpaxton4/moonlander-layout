/** @file
 * Shortcuts layer: Ctrl+number along the number row, plus tab, browser and
 * editor shortcuts, several of them tap dances.
 */

// clang-format off
[LAYER_SHORTCUTS] = LAYOUT_moonlander(
    TO(LAYER_BASE),    _______, LCTL(KC_1), LCTL(KC_2), LCTL(KC_3), LCTL(KC_4),  LCTL(KC_5),              LCTL(KC_6),     LCTL(KC_7), LCTL(KC_8),    LCTL(KC_9),       _______,          _______,        _______,
    _______,           _______, _______,    _______,    _______,    _______,     LCTL(KC_G),              _______,        _______,    LCTL(KC_L),    LCTL(LSFT(KC_C)), TD(DANCE_8),      LCTL(KC_L),     _______,
    _______,           _______, _______,    KC_F6,      LCTL(KC_F), _______,     LCTL(LSFT(KC_G)),        TO(LAYER_BASE), _______,    LALT(KC_LEFT), TD(DANCE_9),      TD(DANCE_10),     LALT(KC_RIGHT), _______,
    _______,           _______, _______,    _______,    _______,    TD(DANCE_7),                                          _______,    TD(DANCE_11),  TD(DANCE_12),     LCTL(LSFT(KC_T)), TD(DANCE_13),   LCTL(KC_W),
    _______,           _______, _______,    _______,    _______,    _______,                                              _______,    _______,       _______,          _______,          _______,        LCTL(LSFT(KC_W)),
                                                        _______,    _______,     _______,                 _______,        _______,    _______
),
    // clang-format on
