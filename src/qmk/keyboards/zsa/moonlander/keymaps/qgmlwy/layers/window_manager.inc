/** @file
 * Window manager layer: modifier and function key chords bound to window
 * manager actions, plus window keys such as Alt+F4.
 */

// clang-format off
[LAYER_WINDOW_MANAGER] = LAYOUT_moonlander(
    TO(LAYER_BASE),              LCTL(LSFT(KC_F13)),      LCTL(LSFT(KC_F15)), LCTL(LSFT(KC_F10)),       LCTL(LSFT(KC_F14)), LCTL(LSFT(KC_F16)),     LCTL(LSFT(KC_F11)),           LCTL(LSFT(KC_F17)), LCTL(LSFT(KC_F12)), _______,                  _______,                  _______,                  _______,                  _______,
    LCTL(LSFT(KC_F18)),          ST_MACRO_38,             ST_MACRO_39,        ST_MACRO_40,              ST_MACRO_41,        _______,                LGUI(KC_C),                   _______,            _______,            LCTL(KC_F1),              LCTL(KC_F2),              LCTL(KC_F3),              LCTL(KC_F4),              _______,
    LALT(LSFT(KC_F1)),           LCTL(LSFT(KC_F1)),       LCTL(LSFT(KC_F3)),  LALT(LCTL(LSFT(KC_F18))), LALT(LSFT(KC_F3)),  _______,                LGUI(KC_W),                   _______,            _______,            LALT(LCTL(LSFT(KC_F2))),  LALT(LCTL(LSFT(KC_F8))),  LALT(LCTL(LSFT(KC_F1))),  LALT(LCTL(LSFT(KC_F3))),  _______,
    LALT(LCTL(LSFT(KC_F14))),    LCTL(LSFT(KC_F7)),       LCTL(LSFT(KC_F9)),  LCTL(LSFT(KC_F6)),        LCTL(LSFT(KC_F8)),  LALT(LSFT(KC_F4)),                                                        TD(DANCE_14),       LALT(LCTL(LSFT(KC_F12))), LALT(LCTL(LSFT(KC_F15))), LALT(LCTL(LSFT(KC_F11))), LALT(LCTL(LSFT(KC_F13))), LALT(LCTL(LSFT(KC_F9))),
    LGUI(LSFT(KC_F1)),           LALT(LCTL(LSFT(KC_F5))), _______,            _______,                  TO(LAYER_BASE),     LALT(LGUI(LSFT(KC_A))),                                                   _______,            _______,                  _______,                  _______,                  _______,                  LALT(LCTL(LSFT(KC_F10))),
                                                                                                        LGUI(KC_PAGE_UP),   LALT(KC_F4),            LGUI(LCTL(KC_ESCAPE)),        _______,            _______,            _______
),
    // clang-format on
