/** @file
 * Status LED settings from the module states and the highest active layer.
 */

// #region Includes

#include QMK_KEYBOARD_H

#include "bracket_pairs/bracket_pairs.h"
#include "key_event_log/key_event_log.h"
#include "layers/layers.h"
#include "status_leds/status_leds.h"

// #endregion

// #region Types

/**
 * Tells if a state of a module is on.
 *
 * @return True while the state is on.
 */
typedef bool (*state_query_t)(void);

/** Status LED of one module state */
typedef struct {
    state_query_t   is_on; /**< Tells if the state is on */
    enum status_led led;   /**< LED that is on while the state is on */
} state_led_t;

// #endregion

// #region Data

/** LEDs that each layer turns on while it is the highest active layer */
static const uint8_t layer_leds[] = {
    [LAYER_BASE]            = 0,
    [LAYER_NAV]             = STATUS_LED_BIT(RIGHT_INNER_LED),
    [LAYER_COMBO_POSITIONS] = 0,
    [LAYER_SPECIAL] =
        STATUS_LED_BIT(LEFT_MIDDLE_LED) | STATUS_LED_BIT(RIGHT_MIDDLE_LED),
    [LAYER_PLAIN] = STATUS_LED_BIT(LEFT_INNER_LED) | STATUS_LED_BIT(RIGHT_OUTER_LED),
};

/** LED of each module state, which comes on with the LEDs of the layer */
static const state_led_t state_leds[] = {
    {.is_on = key_event_log_is_on, .led = LOGGING_LED},
    {.is_on = bracket_pairs_are_open, .led = OPEN_PAIR_LED},
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
    for (size_t index = 0; index < ARRAY_SIZE(state_leds); index++) {
        if (state_leds[index].is_on()) {
            leds |= STATUS_LED_BIT(state_leds[index].led);
        }
    }
    for (uint8_t led = 0; led < STATUS_LED_COUNT; led++) {
        set_status_led((enum status_led)led, (leds & STATUS_LED_BIT(led)) != 0);
    }
}

// #endregion
