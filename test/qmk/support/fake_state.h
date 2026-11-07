/** @file
 * Firmware state of the fake that a test sets or reads: modifiers, status LEDs and
 * the text on the console.
 *
 * reset_fake_qmk() clears all of this state, the call log and the clock.
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

/** Status LEDs of the Moonlander; QMK numbers them 1 to 6 from the left */
#define FAKE_STATUS_LED_COUNT 6

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
 * Fails the test if the console text since the last reset is not the expected
 * text. A failure prints both texts.
 *
 * @param expected Expected text.
 */
void assert_console_text(const char *expected);
