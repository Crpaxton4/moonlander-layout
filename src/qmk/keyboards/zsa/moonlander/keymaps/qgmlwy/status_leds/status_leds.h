/** @file
 * Six status LEDs of the Moonlander: one for logging, the others for the highest
 * active layer.
 *
 * MOONLANDER_USER_LEDS stops the keyboard code from setting the LEDs, and the
 * keyboard then does not call layer_state_set_user(). Thus the housekeeping task
 * sets all six LEDs on each pass. That also overrides the LED sweep at start-up and
 * the LED commands from Oryx, so only logging uses the logging LED.
 */

#pragma once

// #region Includes

#include <stdint.h>

// #endregion

// #region Macros

/** Number of status LEDs */
#define STATUS_LED_COUNT 6

/** LED that is on while logging is on, and that no layer uses */
#define LOGGING_LED LEFT_OUTER_LED

/** Bit of one LED in a set of LEDs */
#define STATUS_LED_BIT(led) ((uint8_t)(1U << (led)))

// #endregion

// #region Enumerations

/** Status LEDs, in the order that QMK numbers them from 1, from the left */
enum status_led {
    LEFT_OUTER_LED   = 0, /**< LED 1, leftmost LED of the left half */
    LEFT_MIDDLE_LED  = 1, /**< LED 2 */
    LEFT_INNER_LED   = 2, /**< LED 3, rightmost LED of the left half */
    RIGHT_INNER_LED  = 3, /**< LED 4, leftmost LED of the right half */
    RIGHT_MIDDLE_LED = 4, /**< LED 5 */
    RIGHT_OUTER_LED  = 5, /**< LED 6, rightmost LED of the right half */
};

// #endregion

// #region Functions

/** Sets each status LED from the logging state and the highest active layer */
void update_status_leds(void);

// #endregion
