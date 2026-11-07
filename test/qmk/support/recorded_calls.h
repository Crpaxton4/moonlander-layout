/** @file
 * Log of the QMK calls that the keymap code makes, and the fake clock, for the
 * tests.
 *
 * Each test starts from an empty log and a clock at 0, then compares the log with
 * the calls that it expects, in order.
 */

#pragma once

// #region Includes

#include <stddef.h>
#include <stdint.h>

// #endregion

// #region Enumerations

/** QMK functions that the fake records */
enum qmk_function {
    CALLED_REGISTER_CODE,     /**< register_code(), argument: keycode */
    CALLED_UNREGISTER_CODE,   /**< unregister_code(), argument: keycode */
    CALLED_TAP_CODE,          /**< tap_code(), argument: keycode */
    CALLED_REGISTER_CODE16,   /**< register_code16(), argument: keycode */
    CALLED_UNREGISTER_CODE16, /**< unregister_code16(), argument: keycode */
    CALLED_TAP_CODE16,        /**< tap_code16(), argument: keycode */
    CALLED_LAYER_ON,          /**< layer_on(), argument: layer */
    CALLED_LAYER_OFF,         /**< layer_off(), argument: layer */
    CALLED_LAYER_MOVE,        /**< layer_move(), argument: layer */
    CALLED_WAIT_MS,           /**< wait_ms(), argument: milliseconds */
    CALLED_SEND_KEYBOARD,     /**< Host driver send_keyboard(), argument: first key */
    CALLED_SEND_NKRO,         /**< Host driver send_nkro(), argument: modifiers */
    CALLED_DEL_MODS,          /**< del_mods(), argument: modifiers */
    CALLED_ADD_MODS,          /**< add_mods(), argument: modifiers */
    CALLED_RGB_MATRIX_MODE_NOEEPROM, /**< rgb_matrix_mode_noeeprom(), argument: mode */
};

// #endregion

// #region Types

/** One QMK call: the function and its argument */
typedef struct {
    enum qmk_function function; /**< Function that the keymap code called */
    uint32_t          argument; /**< Keycode, layer, milliseconds or report byte */
} qmk_call_t;

// #endregion

// #region Functions

/**
 * Empties the log, sets the fake clock to 0 and clears the state in fake_state.h.
 * Layer state goes back to the base layer, QMK's top cursor speed to 0, the key LED
 * brightness to UINT8_MAX, and the host driver to the fake driver.
 */
void reset_fake_qmk(void);

/**
 * Moves the fake clock forward, as time passes between two key events.
 *
 * @param milliseconds Time that passes.
 */
void advance_fake_clock(uint32_t milliseconds);

/**
 * Fails the test if the log is not the expected calls, in the same order.
 *
 * A failure prints both lists, so the output shows the first difference.
 *
 * @param expected Calls that the test expects; NULL when count is 0.
 * @param count    Number of expected calls.
 */
void assert_recorded_calls(const qmk_call_t *expected, size_t count);

// #endregion
