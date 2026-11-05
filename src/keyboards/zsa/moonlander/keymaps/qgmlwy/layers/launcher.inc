/** @file
 * Launcher layer: macros that send the Alt+Ctrl+Shift+grave leader chord
 * followed by a key, on most keys of both halves.
 */

// clang-format off
[LAYER_LAUNCHER] = LAYOUT_moonlander(
    TO(LAYER_BASE),   _______,     ST_MACRO_0,  ST_MACRO_1,     LCTL(KC_I),  LCTL(LSFT(KC_I)), _______,            _______,     _______,     _______,     _______,     _______,          _______,     _______,
    ST_MACRO_2,       ST_MACRO_3,  ST_MACRO_4,  ST_MACRO_5,     ST_MACRO_6,  ST_MACRO_7,       ST_MACRO_8,         ST_MACRO_18, ST_MACRO_19, ST_MACRO_20, ST_MACRO_21, ST_MACRO_22,      ST_MACRO_23, ST_MACRO_24,
    _______,          ST_MACRO_9,  ST_MACRO_10, ST_MACRO_11,    ST_MACRO_12, ST_MACRO_13,      ST_MACRO_14,        ST_MACRO_25, ST_MACRO_26, ST_MACRO_27, ST_MACRO_28, ST_MACRO_29,      ST_MACRO_30, ST_MACRO_31,
    _______,          ST_MACRO_15, _______,     ST_MACRO_16,    _______,     ST_MACRO_17,                                       ST_MACRO_32, ST_MACRO_33, ST_MACRO_34, LGUI(LSFT(KC_P)), ST_MACRO_35, ST_MACRO_36,
    _______,          _______,     _______,     TO(LAYER_BASE), _______,     _______,                                           _______,     _______,     _______,     _______,          _______,     ST_MACRO_37,
                                                                _______,     _______,          _______,            _______,     _______,     _______
),
    // clang-format on
