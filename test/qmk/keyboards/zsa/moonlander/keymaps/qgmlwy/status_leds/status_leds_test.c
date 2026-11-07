/** @file
 * Tests of the status LEDs through the keymap's housekeeping hook.
 *
 * Logging state stays from one case to the next when the program runs all its
 * cases, so each case first sets the state that it needs.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "keymap_introspection.h"

#include "bracket_pairs/bracket_pairs.h"
#include "bracket_pairs/bracket_pairs_logic.h"
#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"
#include "layers/layers.h"
#include "recorded_calls.h"
#include "status_leds/status_leds.h"
#include "unit_test.h"

// #endregion

// #region Macros

/** Layer state with one layer on */
#define ONLY_LAYER(layer) ((layer_state_t)(1U << (layer)))

/** Grid column of V (3,4), the "(" hold */
#define OPENING_PARENTHESIS_COLUMN 4

/** Tap count of a hold */
#define NO_TAP 0

/** Hold time in ms of the Backspace of AC24 */
#define HELD_BACKSPACE_TIME 1000

// #endregion

// #region Static functions

/** Pushes the logging combo, as QMK sends the combo keycode */
static void push_logging_combo(void) {
    keyrecord_t record = {.event = {.pressed = true}};
    process_record_user(LOGGING_TOGGLE, &record);
}

/**
 * Sets logging on or off, then sets the highest layer and runs one housekeeping
 * pass.
 *
 * Logging goes on at time 0, so the idle time starts at 0.
 *
 * @param logging_on True for logging on.
 * @param layer      Highest active layer.
 */
static void show_status(bool logging_on, uint8_t layer) {
    reset_fake_qmk();
    bracket_pairs_clear();
    if (key_event_log_is_on()) {
        push_logging_combo();
    }
    if (logging_on) {
        push_logging_combo();
    }
    layer_state = ONLY_LAYER(layer);
    housekeeping_task_user();
}

/**
 * Sends one key event through process_record_user().
 *
 * @param keycode Keycode that QMK gives the hook.
 * @param pressed True for a push, false for a release.
 */
static void send_key_event(uint16_t keycode, bool pressed) {
    keyrecord_t record = {.event = {.pressed = pressed}};
    record.tap.count   = NO_TAP;
    process_record_user(keycode, &record);
}

/** Holds V (3,4) for "(", which opens a pair, then runs one housekeeping pass */
static void open_pair(void) {
    uint16_t opening_parenthesis = keycode_at_keymap_location(
        LAYER_BASE, LEFT_BOTTOM_LETTER_ROW, OPENING_PARENTHESIS_COLUMN);
    send_key_event(opening_parenthesis, true);
    send_key_event(opening_parenthesis, false);
    housekeeping_task_user();
}

/**
 * Fails the test if the LEDs that are on are not the expected LEDs.
 *
 * @param expected_leds One bit for each expected LED, from STATUS_LED_BIT().
 */
static void assert_leds(uint8_t expected_leds) {
    for (uint8_t led = 0; led < FAKE_STATUS_LED_COUNT; led++) {
        bool expected = (expected_leds & STATUS_LED_BIT(led)) != 0;
        if (fake_status_led_is_on(led) != expected) {
            print_error("Status LED at position %d from the left\n", led);
            assert_int_equal(fake_status_led_is_on(led), expected);
        }
    }
}

static void status_leds_r4_logging_led_on_while_logging_is_on(void **state) {
    (void)state;
    show_status(true, LAYER_BASE);
    assert_leds(STATUS_LED_BIT(LOGGING_LED));
}

static void status_leds_r4_logging_led_off_while_logging_is_off(void **state) {
    (void)state;
    show_status(true, LAYER_BASE);
    show_status(false, LAYER_BASE);
    assert_leds(0);
}

static void status_leds_r4_logging_led_goes_off_at_idle_off(void **state) {
    (void)state;
    show_status(true, LAYER_BASE);
    advance_fake_clock(LOGGING_IDLE_MILLISECONDS);
    housekeeping_task_user();
    assert_leds(0);
}

static void status_leds_r5_no_layer_uses_the_logging_led(void **state) {
    (void)state;
    for (uint8_t layer = 0; layer < keymap_layer_count(); layer++) {
        show_status(false, layer);
        assert_false(fake_status_led_is_on(LOGGING_LED));
    }
}

static void status_leds_r5_logging_led_is_led_1(void **state) {
    (void)state;
    assert_int_equal(LOGGING_LED, LEFT_OUTER_LED);
    assert_int_equal(LEFT_OUTER_LED, 0);
}

static void status_leds_arrows_layer_lights_led_4(void **state) {
    (void)state;
    show_status(false, LAYER_ARROWS);
    assert_leds(STATUS_LED_BIT(RIGHT_INNER_LED));
}

static void status_leds_arrows_layer_and_logging(void **state) {
    (void)state;
    show_status(true, LAYER_ARROWS);
    assert_leds(STATUS_LED_BIT(LOGGING_LED) | STATUS_LED_BIT(RIGHT_INNER_LED));
}

static void status_leds_special_layer_lights_leds_2_and_5(void **state) {
    (void)state;
    show_status(false, LAYER_SPECIAL);
    assert_leds(STATUS_LED_BIT(LEFT_MIDDLE_LED) | STATUS_LED_BIT(RIGHT_MIDDLE_LED));
}

static void status_leds_plain_layer_lights_leds_3_and_6(void **state) {
    (void)state;
    show_status(false, LAYER_PLAIN);
    assert_leds(STATUS_LED_BIT(LEFT_INNER_LED) | STATUS_LED_BIT(RIGHT_OUTER_LED));
}

static void status_leds_r25_open_pair_led_is_led_3(void **state) {
    (void)state;
    assert_int_equal(OPEN_PAIR_LED, LEFT_INNER_LED);
}

static void status_leds_r25_ac22_open_pair_lights_led_3(void **state) {
    (void)state;
    show_status(false, LAYER_BASE);
    open_pair();
    assert_leds(STATUS_LED_BIT(OPEN_PAIR_LED));
}

static void status_leds_r25_ac22_jump_out_turns_led_3_off(void **state) {
    (void)state;
    show_status(false, LAYER_BASE);
    open_pair();
    send_key_event(KC_TAB, true);
    send_key_event(KC_TAB, false);
    housekeeping_task_user();
    assert_leds(0);
}

static void status_leds_r25_ac23_led_3_goes_off_after_idle_time(void **state) {
    (void)state;
    show_status(false, LAYER_BASE);
    open_pair();
    advance_fake_clock(PAIR_IDLE_TIME);
    housekeeping_task_user();
    assert_leds(0);
}

static void status_leds_r25_ac24_held_backspace_turns_led_3_off(void **state) {
    (void)state;
    // Logging is on, as in the flash test
    show_status(true, LAYER_BASE);
    open_pair();
    send_key_event(KC_A, true);
    send_key_event(KC_A, false);
    send_key_event(KC_BACKSPACE, true);
    advance_fake_clock(HELD_BACKSPACE_TIME);
    housekeeping_task_user();
    assert_leds(STATUS_LED_BIT(LOGGING_LED) | STATUS_LED_BIT(OPEN_PAIR_LED));
    send_key_event(KC_BACKSPACE, false);
    housekeeping_task_user();
    assert_leds(STATUS_LED_BIT(LOGGING_LED));
    show_status(false, LAYER_BASE);
}

static void status_leds_r26_open_pair_with_logging_and_arrows(void **state) {
    (void)state;
    show_status(true, LAYER_ARROWS);
    open_pair();
    assert_leds(STATUS_LED_BIT(LOGGING_LED) | STATUS_LED_BIT(OPEN_PAIR_LED) |
                STATUS_LED_BIT(RIGHT_INNER_LED));
    show_status(false, LAYER_BASE);
}

// #endregion

// #region Test runner

/** Cases of this program */
static const struct CMUnitTest status_leds_cases[] = {
    cmocka_unit_test(status_leds_r4_logging_led_on_while_logging_is_on),
    cmocka_unit_test(status_leds_r4_logging_led_off_while_logging_is_off),
    cmocka_unit_test(status_leds_r4_logging_led_goes_off_at_idle_off),
    cmocka_unit_test(status_leds_r5_no_layer_uses_the_logging_led),
    cmocka_unit_test(status_leds_r5_logging_led_is_led_1),
    cmocka_unit_test(status_leds_arrows_layer_lights_led_4),
    cmocka_unit_test(status_leds_arrows_layer_and_logging),
    cmocka_unit_test(status_leds_special_layer_lights_leds_2_and_5),
    cmocka_unit_test(status_leds_plain_layer_lights_leds_3_and_6),
    cmocka_unit_test(status_leds_r25_open_pair_led_is_led_3),
    cmocka_unit_test(status_leds_r25_ac22_open_pair_lights_led_3),
    cmocka_unit_test(status_leds_r25_ac22_jump_out_turns_led_3_off),
    cmocka_unit_test(status_leds_r25_ac23_led_3_goes_off_after_idle_time),
    cmocka_unit_test(status_leds_r25_ac24_held_backspace_turns_led_3_off),
    cmocka_unit_test(status_leds_r26_open_pair_with_logging_and_arrows),
};

int main(int argc, char **argv) {
    select_case(argc, argv);
    return cmocka_run_group_tests(status_leds_cases, NULL, NULL);
}

// #endregion
