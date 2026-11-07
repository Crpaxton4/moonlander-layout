/** @file
 * Status LED settings from the logging state and the highest active layer.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "key_event_log/key_event_log.h"
#include "layers/layers.h"
#include "status_leds/status_leds.h"

// #endregion

// #region Data

/** LEDs that each layer turns on while it is the highest active layer */
static const uint8_t layer_leds[] = {
    [LAYER_BASE]            = 0,
    [LAYER_ARROWS]          = STATUS_LED_BIT(RIGHT_INNER_LED),
    [LAYER_COMBO_POSITIONS] = 0,
    [LAYER_SPECIAL] =
        STATUS_LED_BIT(LEFT_MIDDLE_LED) | STATUS_LED_BIT(RIGHT_MIDDLE_LED),
    [LAYER_PLAIN] = STATUS_LED_BIT(LEFT_INNER_LED) | STATUS_LED_BIT(RIGHT_OUTER_LED),
};

// #endregion

// #region Static functions

/**
 * Sets one status LED.
 *
 * @param led   LED.
 * @param is_on True for on.
 */
static void set_status_led(enum status_led led, bool is_on) {
    // Each STATUS_LED_ macro writes a different pin or expander bit
    switch (led) {
        case LEFT_OUTER_LED:
            STATUS_LED_1(is_on);
            break;
        case LEFT_MIDDLE_LED:
            STATUS_LED_2(is_on);
            break;
        case LEFT_INNER_LED:
            STATUS_LED_3(is_on);
            break;
        case RIGHT_INNER_LED:
            STATUS_LED_4(is_on);
            break;
        case RIGHT_MIDDLE_LED:
            STATUS_LED_5(is_on);
            break;
        case RIGHT_OUTER_LED:
            STATUS_LED_6(is_on);
            break;
    }
}

/**
 * Gives the LEDs of the highest active layer.
 *
 * @return One bit for each LED that is on.
 */
static uint8_t highest_layer_leds(void) {
    uint8_t layer = get_highest_layer(layer_state | default_layer_state);
    if (layer >= ARRAY_SIZE(layer_leds)) {
        return 0;
    }
    return layer_leds[layer];
}

// #endregion

// #region Public functions

void update_status_leds(void) {
    uint8_t leds = highest_layer_leds();
    if (key_event_log_is_on()) {
        leds |= STATUS_LED_BIT(LOGGING_LED);
    }
    for (uint8_t led = 0; led < STATUS_LED_COUNT; led++) {
        set_status_led((enum status_led)led, (leds & STATUS_LED_BIT(led)) != 0);
    }
}

// #endregion
