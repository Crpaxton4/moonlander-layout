/** @file
 * Host replacement for the keyboard header that QMK_KEYBOARD_H names in the
 * firmware build.
 *
 * Real QMK headers give the keycodes and the types, so each value is the firmware
 * value. QMK's own versions of the functions below need the firmware, so
 * fake_quantum.c records each call instead.
 */

#pragma once

/** Rows of the Moonlander key matrix: six for each half */
#define MATRIX_ROWS 12

/** Columns of the Moonlander key matrix */
#define MATRIX_COLS 7

#include <stddef.h>

#include "action.h"
#include "action_layer.h"
#include "process_tap_dance.h"
#include "progmem.h"
#include "quantum_keycodes.h"
#include "timer.h"

/** Number of elements of an array; QMK's version needs GNU C */
#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

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
