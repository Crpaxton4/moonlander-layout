/** @file
 * Key sequences of the qgmlwy keymap's `ST_MACRO_*` keys.
 */

#include QMK_KEYBOARD_H

#include "keycodes/keycodes.h"
#include "macros/macros.h"

bool process_macros(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
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
        default:
            break;
    }
    return true;
}
