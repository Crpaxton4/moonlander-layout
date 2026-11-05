/** @file
 * Key layout, tap dances, macros and per-layer lighting for the qgmlwy keymap.
 */

#include QMK_KEYBOARD_H
#include "version.h"
#define MOON_LED_LEVEL LED_LEVEL
#ifndef ZSA_SAFE_RANGE
#    define ZSA_SAFE_RANGE SAFE_RANGE
#endif

enum custom_keycodes {
    RGB_SLD = ZSA_SAFE_RANGE,
    ST_MACRO_0,
    ST_MACRO_1,
    ST_MACRO_2,
    ST_MACRO_3,
    ST_MACRO_4,
    ST_MACRO_5,
    ST_MACRO_6,
    ST_MACRO_7,
    ST_MACRO_8,
    ST_MACRO_9,
    ST_MACRO_10,
    ST_MACRO_11,
    ST_MACRO_12,
    ST_MACRO_13,
    ST_MACRO_14,
    ST_MACRO_15,
    ST_MACRO_16,
    ST_MACRO_17,
    ST_MACRO_18,
    ST_MACRO_19,
    ST_MACRO_20,
    ST_MACRO_21,
    ST_MACRO_22,
    ST_MACRO_23,
    ST_MACRO_24,
    ST_MACRO_25,
    ST_MACRO_26,
    ST_MACRO_27,
    ST_MACRO_28,
    ST_MACRO_29,
    ST_MACRO_30,
    ST_MACRO_31,
    ST_MACRO_32,
    ST_MACRO_33,
    ST_MACRO_34,
    ST_MACRO_35,
    ST_MACRO_36,
    ST_MACRO_37,
    ST_MACRO_38,
    ST_MACRO_39,
    ST_MACRO_40,
    ST_MACRO_41,
};

/**
 * Tap dance slots, each the index of its action in `tap_dance_actions` and of
 * its record in `dance_state`.
 */
enum tap_dance_codes {
    DANCE_0,
    DANCE_1,
    DANCE_2,
    DANCE_3,
    DANCE_4,
    DANCE_5,
    DANCE_6,
    DANCE_7,
    DANCE_8,
    DANCE_9,
    DANCE_10,
    DANCE_11,
    DANCE_12,
    DANCE_13,
    DANCE_14,
    DANCE_15,
    DANCE_16,
    /** Number of tap dance slots; sizes `dance_state`. */
    DANCE_COUNT,
};

/**
 * Keymap layers, in the order of their index in `keymaps` and `ledmap`.
 */
enum layer {
    /** QGMLWY alphas with mouse keys bottom right. */
    LAYER_BASE = 0,
    /** Base layer with arrows in place of mouse movement. */
    LAYER_ARROWS = 1,
    /** F1 to F20, media and volume keys. */
    LAYER_FUNCTION_MEDIA = 2,
    /** Ctrl+number and tab, browser and editor shortcuts. */
    LAYER_SHORTCUTS = 3,
    /** Alt+Ctrl+Shift+grave leader chord macros. */
    LAYER_LAUNCHER = 4,
    /** Ctrl/Alt/Shift+F13 to F18 bindings and window keys. */
    LAYER_WINDOW_MANAGER = 5,
    /** Mostly transparent gate to the text navigation layer. */
    LAYER_NAVIGATION_GATE = 6,
    /** Ctrl/Shift+arrows and word selection. */
    LAYER_TEXT_NAVIGATION = 7,
    /** Bootloader, NKRO, EEPROM clear, RGB and music keys. */
    LAYER_SYSTEM = 8,
    /** Every key disabled; double-hold either thumb dance to leave. */
    LAYER_LOCK = 9,
    /** Number of layers. */
    LAYER_COUNT,
};

#define DUAL_FUNC_0 LT(6, KC_H)
#define DUAL_FUNC_1 LT(12, KC_T)
#define DUAL_FUNC_2 LT(5, KC_F3)
#define DUAL_FUNC_3 LT(14, KC_F15)

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [LAYER_BASE] = LAYOUT_moonlander(
        KC_ESCAPE,      KC_GRAVE,       KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                                           KC_6,           KC_7,           KC_8,           KC_9,           KC_0,           KC_MINUS,       KC_EQUAL,
        KC_TAB,         KC_Q,           KC_G,           KC_M,           KC_L,           KC_W,           DUAL_FUNC_0,                                    TD(DANCE_1),    KC_Y,           KC_F,           KC_U,           KC_B,           KC_SCLN,        KC_BSLS,
        LSFT(KC_TAB),   KC_D,           KC_S,           KC_T,           KC_N,           KC_R,           DUAL_FUNC_1,                                                                    OSL(LAYER_SHORTCUTS), KC_I,           KC_A,           KC_E,           KC_O,           KC_H,           KC_QUOTE,
        KC_LEFT_SHIFT,  KC_Z,           KC_X,           KC_C,           KC_V,           KC_J,                                           KC_K,           KC_P,           KC_COMMA,       KC_DOT,         KC_SLASH,       TD(DANCE_2),
        TD(DANCE_0),    KC_LEFT_ALT,    OSL(LAYER_FUNCTION_MEDIA), OSL(LAYER_LAUNCHER), OSL(LAYER_WINDOW_MANAGER), KC_ENTER,                                                                                                       TO(LAYER_ARROWS), KC_MS_LEFT,     KC_MS_UP,       KC_MS_DOWN,     KC_MS_RIGHT,    TD(DANCE_3),
        KC_SPACE,       KC_BSPC,        KC_DELETE,                      TD(DANCE_4),    TD(DANCE_5),    KC_MS_BTN1
    ),
    [LAYER_ARROWS] = LAYOUT_moonlander(
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 TO(LAYER_BASE), KC_LEFT,        KC_UP,          KC_DOWN,        KC_RIGHT,       KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_FUNCTION_MEDIA] = LAYOUT_moonlander(
        TO(LAYER_BASE), KC_TRANSPARENT, KC_F1,          KC_F2,          KC_F3,          KC_F4,          KC_F5,                                          KC_F6,          KC_F7,          KC_F8,          KC_F9,          KC_F10,         KC_TRANSPARENT, KC_TRANSPARENT,
        TD(DANCE_6),    KC_TRANSPARENT, KC_F11,         KC_F12,         KC_F13,         KC_F14,         KC_F15,                                         KC_F16,         KC_F17,         KC_F18,         KC_F19,         KC_F20,         KC_TRANSPARENT, KC_TRANSPARENT,
        KC_LEFT_GUI,    KC_TRANSPARENT, KC_TRANSPARENT, LGUI(KC_E),     LGUI(KC_DOT),   KC_TRANSPARENT, LALT(LGUI(KC_LBRC)),                                                                KC_TRANSPARENT, KC_TRANSPARENT, DUAL_FUNC_2,    KC_MEDIA_PLAY_PAUSE,KC_MEDIA_STOP,  DUAL_FUNC_3,    KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_F13,         KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_AUDIO_MUTE,  KC_AUDIO_VOL_UP,KC_AUDIO_VOL_DOWN,KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_SHORTCUTS] = LAYOUT_moonlander(
        TO(LAYER_BASE), KC_TRANSPARENT, LCTL(KC_1),     LCTL(KC_2),     LCTL(KC_3),     LCTL(KC_4),     LCTL(KC_5),                                     LCTL(KC_6),     LCTL(KC_7),     LCTL(KC_8),     LCTL(KC_9),     KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, LCTL(KC_G),                                     KC_TRANSPARENT, KC_TRANSPARENT, LCTL(KC_L),     LCTL(LSFT(KC_C)),TD(DANCE_8),    LCTL(KC_L),     KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_F6,          LCTL(KC_F),     KC_TRANSPARENT, LCTL(LSFT(KC_G)),                                                                TO(LAYER_BASE), KC_TRANSPARENT, LALT(KC_LEFT),  TD(DANCE_9),    TD(DANCE_10),   LALT(KC_RIGHT), KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, TD(DANCE_7),                                    KC_TRANSPARENT, TD(DANCE_11),   TD(DANCE_12),   LCTL(LSFT(KC_T)),TD(DANCE_13),   LCTL(KC_W),
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, LCTL(LSFT(KC_W)),
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_LAUNCHER] = LAYOUT_moonlander(
        TO(LAYER_BASE), KC_TRANSPARENT, ST_MACRO_0,     ST_MACRO_1,     LCTL(KC_I),     LCTL(LSFT(KC_I)),KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        ST_MACRO_2,     ST_MACRO_3,     ST_MACRO_4,     ST_MACRO_5,     ST_MACRO_6,     ST_MACRO_7,     ST_MACRO_8,                                     ST_MACRO_18,    ST_MACRO_19,    ST_MACRO_20,    ST_MACRO_21,    ST_MACRO_22,    ST_MACRO_23,    ST_MACRO_24,
        KC_TRANSPARENT, ST_MACRO_9,     ST_MACRO_10,    ST_MACRO_11,    ST_MACRO_12,    ST_MACRO_13,    ST_MACRO_14,                                                                    ST_MACRO_25,    ST_MACRO_26,    ST_MACRO_27,    ST_MACRO_28,    ST_MACRO_29,    ST_MACRO_30,    ST_MACRO_31,
        KC_TRANSPARENT, ST_MACRO_15,    KC_TRANSPARENT, ST_MACRO_16,    KC_TRANSPARENT, ST_MACRO_17,                                    ST_MACRO_32,    ST_MACRO_33,    ST_MACRO_34,    LGUI(LSFT(KC_P)),ST_MACRO_35,    ST_MACRO_36,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, ST_MACRO_37,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_WINDOW_MANAGER] = LAYOUT_moonlander(
        TO(LAYER_BASE), LCTL(LSFT(KC_F13)),LCTL(LSFT(KC_F15)),LCTL(LSFT(KC_F10)),LCTL(LSFT(KC_F14)),LCTL(LSFT(KC_F16)),LCTL(LSFT(KC_F11)),                                LCTL(LSFT(KC_F17)),LCTL(LSFT(KC_F12)),KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        LCTL(LSFT(KC_F18)),ST_MACRO_38,    ST_MACRO_39,    ST_MACRO_40,    ST_MACRO_41,    KC_TRANSPARENT, LGUI(KC_C),                                     KC_TRANSPARENT, KC_TRANSPARENT, LCTL(KC_F1),    LCTL(KC_F2),    LCTL(KC_F3),    LCTL(KC_F4),    KC_TRANSPARENT,
        LALT(LSFT(KC_F1)),LCTL(LSFT(KC_F1)),LCTL(LSFT(KC_F3)),LALT(LCTL(LSFT(KC_F18))),LALT(LSFT(KC_F3)),KC_TRANSPARENT, LGUI(KC_W),                                                                     KC_TRANSPARENT, KC_TRANSPARENT, LALT(LCTL(LSFT(KC_F2))),LALT(LCTL(LSFT(KC_F8))),LALT(LCTL(LSFT(KC_F1))),LALT(LCTL(LSFT(KC_F3))),KC_TRANSPARENT,
        LALT(LCTL(LSFT(KC_F14))),LCTL(LSFT(KC_F7)),LCTL(LSFT(KC_F9)),LCTL(LSFT(KC_F6)),LCTL(LSFT(KC_F8)),LALT(LSFT(KC_F4)),                                TD(DANCE_14),   LALT(LCTL(LSFT(KC_F12))),LALT(LCTL(LSFT(KC_F15))),LALT(LCTL(LSFT(KC_F11))),LALT(LCTL(LSFT(KC_F13))),LALT(LCTL(LSFT(KC_F9))),
        LGUI(LSFT(KC_F1)),LALT(LCTL(LSFT(KC_F5))),KC_TRANSPARENT, KC_TRANSPARENT, TO(LAYER_BASE), LALT(LGUI(LSFT(KC_A))),                                                                                                KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, LALT(LCTL(LSFT(KC_F10))),
        LGUI(KC_PAGE_UP),LALT(KC_F4),    LGUI(LCTL(KC_ESCAPE)),                KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_NAVIGATION_GATE] = LAYOUT_moonlander(
        TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, OSL(LAYER_TEXT_NAVIGATION), KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_TEXT_NAVIGATION] = LAYOUT_moonlander(
        TO(LAYER_NAVIGATION_GATE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, LCTL(KC_1),     LCTL(KC_2),     LCTL(KC_3),     LCTL(KC_4),     LCTL(KC_X),
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 TO(LAYER_BASE), KC_TRANSPARENT, LCTL(KC_LEFT),  LCTL(KC_UP),    LCTL(KC_DOWN),  LCTL(KC_RIGHT), KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, LCTL(KC_S),     KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, LALT(LSFT(KC_LEFT)),LCTL(LSFT(KC_UP)),LCTL(LSFT(KC_DOWN)),LALT(LSFT(KC_RIGHT)),KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, LSFT(KC_LEFT),  LSFT(KC_UP),    LSFT(KC_DOWN),  LSFT(KC_RIGHT), KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, TO(LAYER_NAVIGATION_GATE), KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_SYSTEM] = LAYOUT_moonlander(
        TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        QK_BOOT,        MAGIC_TOGGLE_NKRO,EE_CLR,         KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 TO(LAYER_BASE), KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, RGB_MODE_FORWARD,RGB_SLD,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                 TO(LAYER_LOCK), MU_TOGG,        MU_NEXT,        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                                                                                                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,
        KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT,                 KC_TRANSPARENT, KC_TRANSPARENT, KC_TRANSPARENT
    ),
    [LAYER_LOCK] = LAYOUT_moonlander(
        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,                                          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,          TD(DANCE_15),                                                                                                   TD(DANCE_16),   KC_NO,          KC_NO,          KC_NO,          KC_NO,          KC_NO,
        KC_NO,          KC_NO,          KC_NO,                          KC_NO,          KC_NO,          KC_NO
    ),
};
// clang-format on

extern rgb_config_t rgb_matrix_config;

RGB hsv_to_rgb_with_value(HSV hsv) {
    RGB   rgb = hsv_to_rgb(hsv);
    float f   = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    return (RGB){f * rgb.r, f * rgb.g, f * rgb.b};
}

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}

// clang-format off
const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [LAYER_BASE] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {237,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {186,197,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {139,16,255}, {5,255,255}, {180,255,255}, {180,255,255}, {180,255,255}, {18,218,204} },

    [LAYER_ARROWS] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {237,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {186,197,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {18,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {18,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {18,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {18,218,204}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {139,16,255}, {5,255,255}, {179,255,255}, {180,255,255}, {180,255,255}, {180,255,255} },

    [LAYER_FUNCTION_MEDIA] = { {0,0,255}, {0,255,228}, {0,0,112}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {92,255,255}, {0,0,0}, {129,255,255}, {92,255,255}, {92,255,255}, {92,255,255}, {139,255,239}, {0,0,0}, {237,255,255}, {92,255,255}, {92,255,255}, {52,235,255}, {0,0,0}, {186,197,255}, {92,255,255}, {92,255,255}, {0,0,0}, {0,0,0}, {92,255,255}, {92,255,255}, {116,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {116,255,255}, {0,0,0}, {180,255,255}, {92,255,255}, {92,255,255}, {116,255,255}, {116,255,255}, {180,255,255}, {92,255,255}, {92,255,255}, {116,255,255}, {116,255,255}, {180,255,255}, {92,255,255}, {92,255,255}, {116,255,255}, {116,255,255}, {180,255,255}, {92,255,255}, {92,255,255}, {0,0,0}, {0,0,0}, {92,255,255}, {92,255,255}, {5,255,255}, {180,255,255}, {180,255,255}, {180,255,255}, {18,218,204} },

    [LAYER_SHORTCUTS] = { {0,0,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {5,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {5,255,255}, {0,0,0}, {5,255,255}, {0,0,0}, {237,255,255}, {5,255,255}, {0,0,0}, {5,255,255}, {0,0,0}, {186,197,255}, {5,255,255}, {0,0,0}, {0,0,0}, {5,255,255}, {5,255,255}, {5,255,255}, {5,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {5,255,255}, {5,255,255}, {0,0,0}, {5,255,255}, {5,255,255}, {5,255,255}, {179,255,255}, {0,0,0}, {5,255,255}, {5,255,255}, {5,255,255}, {179,255,255}, {5,255,255}, {5,255,255}, {5,255,255}, {0,0,0}, {179,255,255}, {5,255,255}, {5,255,255}, {5,255,255}, {5,255,255}, {179,255,255}, {5,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {5,255,255}, {139,16,255}, {5,255,255}, {179,255,255}, {179,255,255}, {179,255,255}, {18,218,204} },

    [LAYER_LAUNCHER] = { {0,0,255}, {139,33,224}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {0,0,0}, {92,255,255}, {237,255,255}, {237,255,255}, {237,255,255}, {237,255,255}, {237,255,255}, {139,33,224}, {237,255,255}, {237,255,255}, {0,0,0}, {186,197,255}, {139,33,224}, {237,255,255}, {237,255,255}, {237,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {237,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {179,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {179,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {179,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {179,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {237,255,255}, {0,0,0}, {237,255,255}, {237,255,255}, {180,255,255}, {179,255,255}, {179,255,255}, {18,218,204} },

    [LAYER_WINDOW_MANAGER] = { {0,0,255}, {187,195,250}, {187,195,250}, {187,195,250}, {187,195,250}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {92,255,255}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {237,255,255}, {187,129,237}, {187,129,237}, {187,129,237}, {187,129,237}, {186,197,255}, {187,252,255}, {0,0,0}, {0,0,0}, {187,129,237}, {187,252,255}, {186,197,255}, {186,197,255}, {187,195,250}, {187,195,250}, {187,195,250}, {187,195,250}, {0,0,0}, {0,0,0}, {0,0,0}, {187,195,250}, {187,195,250}, {0,0,0}, {187,195,250}, {187,195,250}, {187,195,250}, {179,255,255}, {0,0,0}, {187,195,250}, {187,195,250}, {187,195,250}, {179,255,255}, {0,0,0}, {187,195,250}, {187,195,250}, {186,197,255}, {179,255,255}, {0,0,0}, {187,195,250}, {187,195,250}, {186,197,255}, {179,255,255}, {187,252,255}, {0,0,0}, {0,0,0}, {179,255,255}, {187,252,255}, {139,16,255}, {5,255,255}, {179,255,255}, {179,255,255}, {179,255,255}, {18,218,204} },

    [LAYER_NAVIGATION_GATE] = { {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {66,234,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {186,197,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {180,255,255}, {5,255,255}, {180,255,255}, {180,255,255}, {180,255,255}, {18,218,204} },

    [LAYER_TEXT_NAVIGATION] = { {0,0,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {92,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {66,234,255}, {0,0,0}, {0,0,0}, {66,234,255}, {0,0,0}, {186,197,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {66,234,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {66,234,255}, {66,234,255}, {66,234,255}, {66,234,255}, {179,255,255}, {66,234,255}, {66,234,255}, {66,234,255}, {66,234,255}, {179,255,255}, {66,234,255}, {66,234,255}, {66,234,255}, {66,234,255}, {179,255,255}, {66,234,255}, {66,234,255}, {66,234,255}, {66,234,255}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {179,255,255}, {5,255,255}, {179,255,255}, {179,255,255}, {179,255,255}, {18,218,204} },

    [LAYER_SYSTEM] = { {0,0,255}, {0,255,163}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {5,255,255}, {95,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {5,255,255}, {96,255,255}, {0,0,0}, {92,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {237,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {186,197,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {96,255,255}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {97,255,255}, {179,255,255}, {0,0,0}, {0,0,0}, {0,0,0}, {97,255,255}, {0,0,0}, {139,16,255}, {5,255,255}, {179,255,255}, {179,255,255}, {179,255,255}, {18,218,204} },

    [LAYER_LOCK] = { {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,255} },

};
// clang-format on

void set_layer_color(int layer) {
    for (int i = 0; i < RGB_MATRIX_LED_COUNT; i++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][i][0]),
            .s = pgm_read_byte(&ledmap[layer][i][1]),
            .v = pgm_read_byte(&ledmap[layer][i][2]),
        };
        if (!hsv.h && !hsv.s && !hsv.v) {
            rgb_matrix_set_color(i, 0, 0, 0);
        } else {
            RGB rgb = hsv_to_rgb_with_value(hsv);
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
}

bool rgb_matrix_indicators_user(void) {
    if (rawhid_state.rgb_control) {
        return false;
    }
    if (!keyboard_config.disable_layer_led) {
        switch (biton32(layer_state)) {
            case LAYER_BASE:
                set_layer_color(LAYER_BASE);
                break;
            case LAYER_ARROWS:
                set_layer_color(LAYER_ARROWS);
                break;
            case LAYER_FUNCTION_MEDIA:
                set_layer_color(LAYER_FUNCTION_MEDIA);
                break;
            case LAYER_SHORTCUTS:
                set_layer_color(LAYER_SHORTCUTS);
                break;
            case LAYER_LAUNCHER:
                set_layer_color(LAYER_LAUNCHER);
                break;
            case LAYER_WINDOW_MANAGER:
                set_layer_color(LAYER_WINDOW_MANAGER);
                break;
            case LAYER_NAVIGATION_GATE:
                set_layer_color(LAYER_NAVIGATION_GATE);
                break;
            case LAYER_TEXT_NAVIGATION:
                set_layer_color(LAYER_TEXT_NAVIGATION);
                break;
            case LAYER_SYSTEM:
                set_layer_color(LAYER_SYSTEM);
                break;
            case LAYER_LOCK:
                set_layer_color(LAYER_LOCK);
                break;
            default:
                if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
                    rgb_matrix_set_color_all(0, 0, 0);
                }
        }
    } else {
        if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
            rgb_matrix_set_color_all(0, 0, 0);
        }
    }

    return true;
}

/**
 * Per-dance record of the outcome a tap dance resolved to.
 *
 * The finished callback stores the outcome so the reset callback can release
 * exactly what was registered.
 */
typedef struct {
    /** Never read; present only to preserve the record's memory layout. */
    bool is_press_action;
    /** Outcome from dance_step() for the current sequence, or 0 after reset. */
    uint8_t step;
} dance_tracker_t;

enum {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP,
    MORE_TAPS
};

static dance_tracker_t dance_state[DANCE_COUNT];

/** Tap count of a dance tapped or held once. */
#define SINGLE_TAP_COUNT 1

/** Tap count of a dance tapped twice, or tapped once and then held. */
#define DOUBLE_TAP_COUNT 2

uint8_t dance_step(tap_dance_state_t *state);

uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == SINGLE_TAP_COUNT) {
        if (state->interrupted || !state->pressed)
            return SINGLE_TAP;
        else
            return SINGLE_HOLD;
    } else if (state->count == DOUBLE_TAP_COUNT) {
        if (state->interrupted)
            return DOUBLE_SINGLE_TAP;
        else if (state->pressed)
            return DOUBLE_HOLD;
        else
            return DOUBLE_TAP;
    }
    return MORE_TAPS;
}

/**
 * Tap count at which an on-each-tap dance starts repeating its single-tap key.
 *
 * Reaching this count sends the key once per tap so far; each further tap
 * sends it once more.
 */
#define REPEAT_TAP_COUNT 3

void on_dance_0(tap_dance_state_t *state, void *user_data);
void dance_0_finished(tap_dance_state_t *state, void *user_data);
void dance_0_reset(tap_dance_state_t *state, void *user_data);

void on_dance_0(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_LEFT_CTRL);
        tap_code16(KC_LEFT_CTRL);
        tap_code16(KC_LEFT_CTRL);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_LEFT_CTRL);
    }
}

void dance_0_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_0].step = dance_step(state);
    switch (dance_state[DANCE_0].step) {
        case SINGLE_TAP:
            register_code16(KC_LEFT_CTRL);
            break;
        case SINGLE_HOLD:
            register_code16(KC_LEFT_CTRL);
            break;
        case DOUBLE_TAP:
            register_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_LEFT_CTRL);
            register_code16(KC_LEFT_CTRL);
    }
}

void dance_0_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_0].step) {
        case SINGLE_TAP:
            unregister_code16(KC_LEFT_CTRL);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_LEFT_CTRL);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_LEFT_GUI);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_LEFT_CTRL);
            break;
    }
    dance_state[DANCE_0].step = 0;
}
void dance_1_finished(tap_dance_state_t *state, void *user_data);
void dance_1_reset(tap_dance_state_t *state, void *user_data);

void dance_1_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_1].step = dance_step(state);
    switch (dance_state[DANCE_1].step) {
        case SINGLE_TAP:
            layer_move(LAYER_NAVIGATION_GATE);
            break;
        case DOUBLE_TAP:
            layer_move(LAYER_SYSTEM);
            break;
        case DOUBLE_SINGLE_TAP:
            layer_move(LAYER_NAVIGATION_GATE);
            break;
    }
}

void dance_1_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_1].step = 0;
}
void on_dance_2(tap_dance_state_t *state, void *user_data);
void dance_2_finished(tap_dance_state_t *state, void *user_data);
void dance_2_reset(tap_dance_state_t *state, void *user_data);

void on_dance_2(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
        tap_code16(KC_LPRN);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_LPRN);
    }
}

void dance_2_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_2].step = dance_step(state);
    switch (dance_state[DANCE_2].step) {
        case SINGLE_TAP:
            register_code16(KC_LPRN);
            break;
        case SINGLE_HOLD:
            register_code16(KC_LCBR);
            break;
        case DOUBLE_TAP:
            register_code16(KC_LBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_LPRN);
            register_code16(KC_LPRN);
    }
}

void dance_2_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_2].step) {
        case SINGLE_TAP:
            unregister_code16(KC_LPRN);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_LCBR);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_LBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_LPRN);
            break;
    }
    dance_state[DANCE_2].step = 0;
}
void on_dance_3(tap_dance_state_t *state, void *user_data);
void dance_3_finished(tap_dance_state_t *state, void *user_data);
void dance_3_reset(tap_dance_state_t *state, void *user_data);

void on_dance_3(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_RPRN);
        tap_code16(KC_RPRN);
        tap_code16(KC_RPRN);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_RPRN);
    }
}

void dance_3_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_3].step = dance_step(state);
    switch (dance_state[DANCE_3].step) {
        case SINGLE_TAP:
            register_code16(KC_RPRN);
            break;
        case SINGLE_HOLD:
            register_code16(KC_RCBR);
            break;
        case DOUBLE_TAP:
            register_code16(KC_RBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_RPRN);
            register_code16(KC_RPRN);
    }
}

void dance_3_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_3].step) {
        case SINGLE_TAP:
            unregister_code16(KC_RPRN);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_RCBR);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_RBRC);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_RPRN);
            break;
    }
    dance_state[DANCE_3].step = 0;
}
void on_dance_4(tap_dance_state_t *state, void *user_data);
void dance_4_finished(tap_dance_state_t *state, void *user_data);
void dance_4_reset(tap_dance_state_t *state, void *user_data);

void on_dance_4(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN3);
        tap_code16(KC_MS_BTN3);
        tap_code16(KC_MS_BTN3);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN3);
    }
}

void dance_4_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_4].step = dance_step(state);
    switch (dance_state[DANCE_4].step) {
        case SINGLE_TAP:
            register_code16(KC_MS_BTN3);
            break;
        case SINGLE_HOLD:
            register_code16(KC_MS_WH_DOWN);
            break;
        case DOUBLE_TAP:
            register_code16(KC_MS_BTN3);
            register_code16(KC_MS_BTN3);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_MS_WH_LEFT);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_MS_BTN3);
            register_code16(KC_MS_BTN3);
    }
}

void dance_4_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_4].step) {
        case SINGLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_MS_WH_DOWN);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_MS_WH_LEFT);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_MS_BTN3);
            break;
    }
    dance_state[DANCE_4].step = 0;
}
void on_dance_5(tap_dance_state_t *state, void *user_data);
void dance_5_finished(tap_dance_state_t *state, void *user_data);
void dance_5_reset(tap_dance_state_t *state, void *user_data);

void on_dance_5(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN2);
        tap_code16(KC_MS_BTN2);
        tap_code16(KC_MS_BTN2);
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(KC_MS_BTN2);
    }
}

void dance_5_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_5].step = dance_step(state);
    switch (dance_state[DANCE_5].step) {
        case SINGLE_TAP:
            register_code16(KC_MS_BTN2);
            break;
        case SINGLE_HOLD:
            register_code16(KC_MS_WH_UP);
            break;
        case DOUBLE_TAP:
            register_code16(KC_MS_BTN2);
            register_code16(KC_MS_BTN2);
            break;
        case DOUBLE_HOLD:
            register_code16(KC_MS_WH_RIGHT);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_MS_BTN2);
            register_code16(KC_MS_BTN2);
    }
}

void dance_5_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_5].step) {
        case SINGLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_MS_WH_UP);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_MS_WH_RIGHT);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_MS_BTN2);
            break;
    }
    dance_state[DANCE_5].step = 0;
}
void dance_6_finished(tap_dance_state_t *state, void *user_data);
void dance_6_reset(tap_dance_state_t *state, void *user_data);

void dance_6_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_6].step = dance_step(state);
    switch (dance_state[DANCE_6].step) {
        case SINGLE_HOLD:
            register_code16(LALT(LCTL(KC_DELETE)));
            break;
        case DOUBLE_TAP:
            register_code16(LALT(LCTL(LSFT(KC_DELETE))));
            break;
        case DOUBLE_HOLD:
            register_code16(KC_SYSTEM_POWER);
            break;
    }
}

void dance_6_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_6].step) {
        case SINGLE_HOLD:
            unregister_code16(LALT(LCTL(KC_DELETE)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LALT(LCTL(LSFT(KC_DELETE))));
            break;
        case DOUBLE_HOLD:
            unregister_code16(KC_SYSTEM_POWER);
            break;
    }
    dance_state[DANCE_6].step = 0;
}
void on_dance_7(tap_dance_state_t *state, void *user_data);
void dance_7_finished(tap_dance_state_t *state, void *user_data);
void dance_7_reset(tap_dance_state_t *state, void *user_data);

void on_dance_7(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_D));
        tap_code16(LCTL(KC_D));
        tap_code16(LCTL(KC_D));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_D));
    }
}

void dance_7_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_7].step = dance_step(state);
    switch (dance_state[DANCE_7].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_D));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_D)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_D));
            register_code16(LCTL(KC_D));
    }
}

void dance_7_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_7].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_D));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_D)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_D));
            break;
    }
    dance_state[DANCE_7].step = 0;
}
void on_dance_8(tap_dance_state_t *state, void *user_data);
void dance_8_finished(tap_dance_state_t *state, void *user_data);
void dance_8_reset(tap_dance_state_t *state, void *user_data);

void on_dance_8(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_J)));
        tap_code16(LCTL(LSFT(KC_J)));
        tap_code16(LCTL(LSFT(KC_J)));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_J)));
    }
}

void dance_8_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_8].step = dance_step(state);
    switch (dance_state[DANCE_8].step) {
        case SINGLE_TAP:
            register_code16(LCTL(LSFT(KC_J)));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_I)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(LSFT(KC_J)));
            register_code16(LCTL(LSFT(KC_J)));
    }
}

void dance_8_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_8].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_J)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_I)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_J)));
            break;
    }
    dance_state[DANCE_8].step = 0;
}
void on_dance_9(tap_dance_state_t *state, void *user_data);
void dance_9_finished(tap_dance_state_t *state, void *user_data);
void dance_9_reset(tap_dance_state_t *state, void *user_data);

void on_dance_9(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_RBRC));
        tap_code16(LCTL(KC_RBRC));
        tap_code16(LCTL(KC_RBRC));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_RBRC));
    }
}

void dance_9_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_9].step = dance_step(state);
    switch (dance_state[DANCE_9].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_RBRC));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_LBRC));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_RBRC));
            register_code16(LCTL(KC_RBRC));
    }
}

void dance_9_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_9].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_RBRC));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_LBRC));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_RBRC));
            break;
    }
    dance_state[DANCE_9].step = 0;
}
void on_dance_10(tap_dance_state_t *state, void *user_data);
void dance_10_finished(tap_dance_state_t *state, void *user_data);
void dance_10_reset(tap_dance_state_t *state, void *user_data);

void on_dance_10(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_R));
        tap_code16(LCTL(KC_R));
        tap_code16(LCTL(KC_R));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_R));
    }
}

void dance_10_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_10].step = dance_step(state);
    switch (dance_state[DANCE_10].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_R));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(LSFT(KC_R)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_R));
            register_code16(LCTL(KC_R));
    }
}

void dance_10_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_10].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_R));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(LSFT(KC_R)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_R));
            break;
    }
    dance_state[DANCE_10].step = 0;
}
void on_dance_11(tap_dance_state_t *state, void *user_data);
void dance_11_finished(tap_dance_state_t *state, void *user_data);
void dance_11_reset(tap_dance_state_t *state, void *user_data);

void on_dance_11(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_TAB)));
        tap_code16(LCTL(LSFT(KC_TAB)));
        tap_code16(LCTL(LSFT(KC_TAB)));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(LSFT(KC_TAB)));
    }
}

void dance_11_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_11].step = dance_step(state);
    switch (dance_state[DANCE_11].step) {
        case SINGLE_TAP:
            register_code16(LCTL(LSFT(KC_TAB)));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_1));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_PAGE_UP)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(LSFT(KC_TAB)));
            register_code16(LCTL(LSFT(KC_TAB)));
    }
}

void dance_11_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_11].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_TAB)));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_1));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_PAGE_UP)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(LSFT(KC_TAB)));
            break;
    }
    dance_state[DANCE_11].step = 0;
}
void on_dance_12(tap_dance_state_t *state, void *user_data);
void dance_12_finished(tap_dance_state_t *state, void *user_data);
void dance_12_reset(tap_dance_state_t *state, void *user_data);

void on_dance_12(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_T));
        tap_code16(LCTL(KC_T));
        tap_code16(LCTL(KC_T));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_T));
    }
}

void dance_12_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_12].step = dance_step(state);
    switch (dance_state[DANCE_12].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_T));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_N));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_N)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_T));
            register_code16(LCTL(KC_T));
    }
}

void dance_12_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_12].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_T));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_N));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_N)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_T));
            break;
    }
    dance_state[DANCE_12].step = 0;
}
void on_dance_13(tap_dance_state_t *state, void *user_data);
void dance_13_finished(tap_dance_state_t *state, void *user_data);
void dance_13_reset(tap_dance_state_t *state, void *user_data);

void on_dance_13(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_TAB));
        tap_code16(LCTL(KC_TAB));
        tap_code16(LCTL(KC_TAB));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LCTL(KC_TAB));
    }
}

void dance_13_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_13].step = dance_step(state);
    switch (dance_state[DANCE_13].step) {
        case SINGLE_TAP:
            register_code16(LCTL(KC_TAB));
            break;
        case DOUBLE_TAP:
            register_code16(LCTL(KC_9));
            break;
        case DOUBLE_HOLD:
            register_code16(LCTL(LSFT(KC_PGDN)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LCTL(KC_TAB));
            register_code16(LCTL(KC_TAB));
    }
}

void dance_13_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_13].step) {
        case SINGLE_TAP:
            unregister_code16(LCTL(KC_TAB));
            break;
        case DOUBLE_TAP:
            unregister_code16(LCTL(KC_9));
            break;
        case DOUBLE_HOLD:
            unregister_code16(LCTL(LSFT(KC_PGDN)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LCTL(KC_TAB));
            break;
    }
    dance_state[DANCE_13].step = 0;
}
void on_dance_14(tap_dance_state_t *state, void *user_data);
void dance_14_finished(tap_dance_state_t *state, void *user_data);
void dance_14_reset(tap_dance_state_t *state, void *user_data);

void on_dance_14(tap_dance_state_t *state, void *user_data) {
    if (state->count == REPEAT_TAP_COUNT) {
        tap_code16(LGUI(KC_F5));
        tap_code16(LGUI(KC_F5));
        tap_code16(LGUI(KC_F5));
    }
    if (state->count > REPEAT_TAP_COUNT) {
        tap_code16(LGUI(KC_F5));
    }
}

void dance_14_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_14].step = dance_step(state);
    switch (dance_state[DANCE_14].step) {
        case SINGLE_TAP:
            register_code16(LGUI(KC_F5));
            break;
        case DOUBLE_TAP:
            register_code16(LGUI(LSFT(KC_F5)));
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(LGUI(KC_F5));
            register_code16(LGUI(KC_F5));
    }
}

void dance_14_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (dance_state[DANCE_14].step) {
        case SINGLE_TAP:
            unregister_code16(LGUI(KC_F5));
            break;
        case DOUBLE_TAP:
            unregister_code16(LGUI(LSFT(KC_F5)));
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(LGUI(KC_F5));
            break;
    }
    dance_state[DANCE_14].step = 0;
}
void dance_15_finished(tap_dance_state_t *state, void *user_data);
void dance_15_reset(tap_dance_state_t *state, void *user_data);

void dance_15_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_15].step = dance_step(state);
    switch (dance_state[DANCE_15].step) {
        case DOUBLE_HOLD:
            layer_move(LAYER_BASE);
            break;
    }
}

void dance_15_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_15].step = 0;
}
void dance_16_finished(tap_dance_state_t *state, void *user_data);
void dance_16_reset(tap_dance_state_t *state, void *user_data);

void dance_16_finished(tap_dance_state_t *state, void *user_data) {
    dance_state[DANCE_16].step = dance_step(state);
    switch (dance_state[DANCE_16].step) {
        case DOUBLE_HOLD:
            layer_move(LAYER_BASE);
            break;
    }
}

void dance_16_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    dance_state[DANCE_16].step = 0;
}

tap_dance_action_t tap_dance_actions[] = {
    [DANCE_0] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_0, dance_0_finished, dance_0_reset),
    [DANCE_1] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_1_finished, dance_1_reset),
    [DANCE_2] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_2, dance_2_finished, dance_2_reset),
    [DANCE_3] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_3, dance_3_finished, dance_3_reset),
    [DANCE_4] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_4, dance_4_finished, dance_4_reset),
    [DANCE_5] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_5, dance_5_finished, dance_5_reset),
    [DANCE_6] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_6_finished, dance_6_reset),
    [DANCE_7] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_7, dance_7_finished, dance_7_reset),
    [DANCE_8] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_8, dance_8_finished, dance_8_reset),
    [DANCE_9] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_9, dance_9_finished, dance_9_reset),
    [DANCE_10] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_10, dance_10_finished, dance_10_reset),
    [DANCE_11] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_11, dance_11_finished, dance_11_reset),
    [DANCE_12] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_12, dance_12_finished, dance_12_reset),
    [DANCE_13] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_13, dance_13_finished, dance_13_reset),
    [DANCE_14] =
        ACTION_TAP_DANCE_FN_ADVANCED(on_dance_14, dance_14_finished, dance_14_reset),
    [DANCE_15] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_15_finished, dance_15_reset),
    [DANCE_16] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dance_16_finished, dance_16_reset),
};

/**
 * rgblight's mode number for a static single colour, RGBLIGHT_MODE_STATIC_LIGHT.
 *
 * Without rgblight, QMK maps rgblight_mode() to rgb_matrix_mode(), where this
 * number selects RGB_MATRIX_SOLID_COLOR.
 */
#define STATIC_LIGHT_MODE 1

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case QK_MODS ... QK_MODS_MAX:
            // Mouse and consumer keys (volume, media) with modifiers work
            // inconsistently across operating systems, this makes sure that modifiers
            // are always applied to the key that was pressed.
            if (IS_MOUSE_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode)) ||
                IS_CONSUMER_KEYCODE(QK_MODS_GET_BASIC_KEYCODE(keycode))) {
                if (record->event.pressed) {
                    add_mods(QK_MODS_GET_MODS(keycode));
                    send_keyboard_report();
                    wait_ms(2);
                    register_code(QK_MODS_GET_BASIC_KEYCODE(keycode));
                    return false;
                } else {
                    wait_ms(2);
                    del_mods(QK_MODS_GET_MODS(keycode));
                }
            }
            break;
        case ST_MACRO_0:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_1)))));
            }
            break;
        case ST_MACRO_1:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_2)))));
            }
            break;
        case ST_MACRO_2:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_X)))));
            }
            break;
        case ST_MACRO_3:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_Q)))));
            }
            break;
        case ST_MACRO_4:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_EQUAL)))));
            }
            break;
        case ST_MACRO_5:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_E)))));
            }
            break;
        case ST_MACRO_6:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_R)))));
            }
            break;
        case ST_MACRO_7:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_T)))));
            }
            break;
        case ST_MACRO_8:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_C)))));
            }
            break;
        case ST_MACRO_9:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_A)))));
            }
            break;
        case ST_MACRO_10:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_S)))));
            }
            break;
        case ST_MACRO_11:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_D)))));
            }
            break;
        case ST_MACRO_12:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_F)))));
            }
            break;
        case ST_MACRO_13:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_G)))));
            }
            break;
        case ST_MACRO_14:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_V)))));
            }
            break;
        case ST_MACRO_15:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_Z)))));
            }
            break;
        case ST_MACRO_16:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_SLASH)))));
            }
            break;
        case ST_MACRO_17:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_B)))));
            }
            break;
        case ST_MACRO_18:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_TAP(X_N))));
            }
            break;
        case ST_MACRO_19:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_Y)))));
            }
            break;
        case ST_MACRO_20:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_U)))));
            }
            break;
        case ST_MACRO_21:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_I)))));
            }
            break;
        case ST_MACRO_22:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_O)))));
            }
            break;
        case ST_MACRO_23:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_P)))));
            }
            break;
        case ST_MACRO_24:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_BSLS)))));
            }
            break;
        case ST_MACRO_25:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_N)))));
            }
            break;
        case ST_MACRO_26:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_H)))));
            }
            break;
        case ST_MACRO_27:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_J)))));
            }
            break;
        case ST_MACRO_28:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_K)))));
            }
            break;
        case ST_MACRO_29:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_L)))));
            }
            break;
        case ST_MACRO_30:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_SCLN)))));
            }
            break;
        case ST_MACRO_31:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_RBRC)))));
            }
            break;
        case ST_MACRO_32:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))));
            }
            break;
        case ST_MACRO_33:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_LEFT)))));
            }
            break;
        case ST_MACRO_34:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_UP)))));
            }
            break;
        case ST_MACRO_35:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_RIGHT)))));
            }
            break;
        case ST_MACRO_36:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_DOWN)))));
            }
            break;
        case ST_MACRO_37:
            if (record->event.pressed) {
                SEND_STRING(SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_GRAVE)))) SS_DELAY(100)
                                SS_LALT(SS_LCTL(SS_LSFT(SS_TAP(X_LBRC)))));
            }
            break;
        case ST_MACRO_38:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_TAP(X_F1)) SS_DELAY(250) SS_LCTL(SS_TAP(X_F1)));
            }
            break;
        case ST_MACRO_39:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_TAP(X_F2)) SS_DELAY(250) SS_LCTL(SS_TAP(X_F2)));
            }
            break;
        case ST_MACRO_40:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_TAP(X_F3)) SS_DELAY(250) SS_LCTL(SS_TAP(X_F3)));
            }
            break;
        case ST_MACRO_41:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_TAP(X_F4)) SS_DELAY(250) SS_LCTL(SS_TAP(X_F4)));
            }
            break;

        case DUAL_FUNC_0:
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    register_code16(LCTL(KC_C));
                } else {
                    unregister_code16(LCTL(KC_C));
                }
            } else {
                if (record->event.pressed) {
                    register_code16(LCTL(LSFT(KC_C)));
                } else {
                    unregister_code16(LCTL(LSFT(KC_C)));
                }
            }
            return false;
        case DUAL_FUNC_1:
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    register_code16(LCTL(KC_V));
                } else {
                    unregister_code16(LCTL(KC_V));
                }
            } else {
                if (record->event.pressed) {
                    register_code16(LCTL(LSFT(KC_V)));
                } else {
                    unregister_code16(LCTL(LSFT(KC_V)));
                }
            }
            return false;
        case DUAL_FUNC_2:
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    register_code16(KC_MEDIA_PREV_TRACK);
                } else {
                    unregister_code16(KC_MEDIA_PREV_TRACK);
                }
            } else {
                if (record->event.pressed) {
                    register_code16(KC_MEDIA_REWIND);
                } else {
                    unregister_code16(KC_MEDIA_REWIND);
                }
            }
            return false;
        case DUAL_FUNC_3:
            if (record->tap.count > 0) {
                if (record->event.pressed) {
                    register_code16(KC_MEDIA_NEXT_TRACK);
                } else {
                    unregister_code16(KC_MEDIA_NEXT_TRACK);
                }
            } else {
                if (record->event.pressed) {
                    register_code16(KC_MEDIA_FAST_FORWARD);
                } else {
                    unregister_code16(KC_MEDIA_FAST_FORWARD);
                }
            }
            return false;
        case RGB_SLD:
            if (rawhid_state.rgb_control) {
                return false;
            }
            if (record->event.pressed) {
                rgblight_mode(STATIC_LIGHT_MODE);
            }
            return false;
    }
    return true;
}
