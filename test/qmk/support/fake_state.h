/** @file
 * Firmware state of the fake that a test sets or reads: modifiers, status LEDs, key
 * LEDs and the text on the console.
 *
 * reset_fake_qmk() clears all of this state, the call log and the clock.
 */

#pragma once

// #region Includes

#include <stdbool.h>
#include <stdint.h>

#include "color.h"

// #endregion

// #region Macros

/** Status LEDs of the Moonlander; QMK numbers them 1 to 6 from the left */
#define FAKE_STATUS_LED_COUNT 6

// #endregion

// #region Functions

/**
 * Sets the modifiers that get_mods() gives.
 *
 * @param modifiers Active modifier bits.
 */
void set_fake_modifiers(uint8_t modifiers);

/**
 * Sets the modifiers that get_oneshot_mods() gives.
 *
 * @param modifiers One-shot modifier bits.
 */
void set_fake_one_shot_modifiers(uint8_t modifiers);

/**
 * Sets one status LED, as the STATUS_LED_1() to STATUS_LED_6() macros do.
 *
 * @param position Position of the LED from the left, 0 for STATUS_LED_1().
 * @param is_on    True for on.
 */
void set_fake_status_led(uint8_t position, bool is_on);

/**
 * Tells if one status LED is on.
 *
 * @param position Position of the LED from the left, 0 for STATUS_LED_1().
 * @return True if the last setting of that LED was on.
 */
bool fake_status_led_is_on(uint8_t position);

/**
 * Sets the brightness of the RGB matrix that rgb_matrix_get_val() gives.
 *
 * @param brightness Brightness from 0 to UINT8_MAX; reset_fake_qmk() sets UINT8_MAX.
 */
void set_fake_key_light_brightness(uint8_t brightness);

/**
 * Gives the last color of one key LED, as rgb_matrix_set_color_all() sets it.
 *
 * @param led Index of the LED in the RGB matrix.
 * @return Color of the LED; black after reset_fake_qmk().
 */
rgb_t fake_key_light_color(uint8_t led);

/**
 * Fails the test if the console text since the last reset is not the expected
 * text. A failure prints both texts.
 *
 * @param expected Expected text.
 */
void assert_console_text(const char *expected);

// #endregion
