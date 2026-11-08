/** @file
 * Host replacement for the keyboard header that QMK_KEYBOARD_H names in the
 * firmware build.
 *
 * Real QMK headers give the keycodes and the types, so each value is the firmware
 * value. QMK's own versions of the functions below need the firmware, so
 * fake_quantum.c records each call instead.
 */

#pragma once

// #region Macros

// QMK headers read the matrix size and keep this ARRAY_SIZE, so the macros come
// before the includes

/** Rows of the Moonlander key matrix: six for each half */
#define MATRIX_ROWS 12

/** Columns of the Moonlander key matrix */
#define MATRIX_COLS 7

/** Number of key LEDs of the Moonlander, as the keyboard data of QMK gives them */
#define RGB_MATRIX_LED_COUNT 72

/** Number of elements of an array; QMK headers keep it, because their version needs GNU
 * C */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

/** Sets status LED 1, the leftmost LED, as moonlander.h does on the keyboard */
#define STATUS_LED_1(status) set_fake_status_led(0, (status))
/** Sets status LED 2 */
#define STATUS_LED_2(status) set_fake_status_led(1, (status))
/** Sets status LED 3, the right LED of the left half */
#define STATUS_LED_3(status) set_fake_status_led(2, (status))
/** Sets status LED 4, the left LED of the right half */
#define STATUS_LED_4(status) set_fake_status_led(3, (status))
/** Sets status LED 5 */
#define STATUS_LED_5(status) set_fake_status_led(4, (status))
/** Sets status LED 6, the rightmost LED */
#define STATUS_LED_6(status) set_fake_status_led(5, (status))

// #endregion

// #region Includes

#include <stddef.h>

#include "fake_state.h"

#include "action.h"
#include "action_layer.h"
#include "action_tapping.h"
#include "action_util.h"
#include "host.h"
#include "keymap_common.h"
#include "process_combo.h"
#include "progmem.h"
#include "quantum_keycodes.h"
#include "rgb_matrix.h"
#include "timer.h"

// #endregion

// #region Functions

/**
 * Adds formatted text to the console text that the tests read, as QMK's uprintf()
 * writes to the console.
 *
 * QMK makes uprintf() a macro of its printf(). A function lets the tests read the
 * text.
 *
 * @param format printf() format.
 * @param ...    Values of the format.
 * @return Number of characters, as printf().
 */
int uprintf(const char *format, ...) __attribute__((format(printf, 1, 2)));

/**
 * Records a press of a keycode with modifiers, as QMK's register_code16().
 *
 * @param code Keycode, with its modifier bits.
 */
void register_code16(uint16_t code);

/**
 * Records a release of a keycode with modifiers, as QMK's unregister_code16().
 *
 * @param code Keycode, with its modifier bits.
 */
void unregister_code16(uint16_t code);

/**
 * Records a tap of a keycode with modifiers, as QMK's tap_code16().
 *
 * @param code Keycode, with its modifier bits.
 */
void tap_code16(uint16_t code);

/**
 * Records a wait and moves the fake clock forward by that time.
 *
 * QMK makes wait_ms() a macro on ChibiOS. A function gives the tests a record.
 *
 * @param milliseconds Time to wait.
 */
void wait_ms(uint32_t milliseconds);

/**
 * Keymap hook that QMK calls for each key event.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return False when the keymap fully handled the event, true otherwise.
 */
bool process_record_user(uint16_t keycode, keyrecord_t *record);

/**
 * Keymap hook that QMK calls after the action of each key event that
 * process_record_user() let pass.
 *
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 */
void post_process_record_user(uint16_t keycode, keyrecord_t *record);

// #endregion
