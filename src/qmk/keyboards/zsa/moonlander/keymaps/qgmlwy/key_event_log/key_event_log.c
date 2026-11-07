/** @file
 * Records of the key event log on the QMK console.
 *
 * Tap dances, dual-function keys and QMK's tap code send keys from callbacks,
 * often a push and a release in one pass of the main loop. Thus the output records
 * come from a host driver that sends each report and then compares it with the
 * last report. A comparison in the housekeeping task would not see those keys.
 */

#include QMK_KEYBOARD_H

#include <string.h>

#include "key_event_log/key_event_log.h"
#include "keycodes/keycodes.h"

/** Logging state; all zero at power-up, so logging is off */
static logging_t logging;

/** Host driver that QMK set, which sends the reports to the host */
static host_driver_t *host_driver;

/** Copy of host_driver that also writes the output records */
static host_driver_t logging_driver;

/**
 * Gives the time for a record. uint32_t is unsigned long on the keyboard and
 * unsigned int on a test host, so one printf() format needs a fixed type.
 *
 * @return Time since power-up in ms.
 */
static unsigned long record_time(void) {
    return timer_read32();
}

/**
 * Writes a state record.
 *
 * @param state New logging state.
 */
static void write_state_record(enum logging_state state) {
    uprintf("KL,S,%lu,%s\n", record_time(), logging_state_name(state));
}

/**
 * Writes an input record.
 *
 * @param keycode Keycode of the key event, after layer resolution.
 * @param record  Key event.
 */
static void write_input_record(uint16_t keycode, const keyrecord_t *record) {
    uprintf("KL,I,%lu,%c,%u,%u,%u,%04X,%02X,%02X,%u,%u\n", record_time(),
            key_event_direction(record->event.pressed), record->event.key.row,
            record->event.key.col, get_highest_layer(layer_state | default_layer_state),
            keycode, get_mods(), get_oneshot_mods(), record->tap.count,
            record->tap.interrupted);
}

/**
 * Writes an output record if the report changed and logging is on.
 *
 * Logging off still keeps the report, so the first output record after logging
 * goes on shows a real change.
 *
 * @param report Report that the keyboard sent.
 */
static void log_report(const logged_report_t *report) {
    bool changed = logging_report_changed(&logging, report);
    if (!changed || !logging.is_on) {
        return;
    }
    uprintf("KL,O,%lu,%02X", record_time(), report->modifiers);
    for (size_t slot = 0; slot < LOGGED_REPORT_KEY_COUNT; slot++) {
        uprintf(",%02X", report->keys[slot]);
    }
    uprintf("\n");
}

/**
 * Sends a 6-key report to the host, then logs it.
 *
 * @param report Report that QMK sends.
 */
static void send_logged_keyboard_report(report_keyboard_t *report) {
    // Host gets the report first, so a slow console does not delay the keys
    host_driver->send_keyboard(report);
    logged_report_t logged = {.modifiers = report->mods};
    memcpy(logged.keys, report->keys, sizeof(logged.keys));
    log_report(&logged);
}

/**
 * Sends an NKRO report to the host, then logs it.
 *
 * @param report Report that QMK sends.
 */
static void send_logged_nkro_report(report_nkro_t *report) {
    host_driver->send_nkro(report);
    logged_report_t logged = {.modifiers = report->mods};
    keys_from_bitmap(logged.keys, report->bits, sizeof(report->bits));
    log_report(&logged);
}

/** Puts the logging driver in place of the host driver that QMK set */
static void use_logging_driver(void) {
    host_driver_t *driver = host_get_driver();
    if (driver == NULL || driver == &logging_driver) {
        return;
    }
    host_driver                  = driver;
    logging_driver               = *driver;
    logging_driver.send_keyboard = send_logged_keyboard_report;
    logging_driver.send_nkro     = send_logged_nkro_report;
    host_set_driver(&logging_driver);
}

bool process_key_event_log(uint16_t keycode, keyrecord_t *record) {
    if (logging.is_on) {
        write_input_record(keycode, record);
    }
    if (record->event.pressed) {
        logging_note_push(&logging, timer_read32());
    }
    if (keycode != LOGGING_TOGGLE) {
        return true;
    }
    if (record->event.pressed) {
        write_state_record(logging_toggle(&logging, timer_read32()));
    }
    return false;
}

void key_event_log_housekeeping(void) {
    use_logging_driver();
    if (logging_idle_ended(&logging, timer_read32())) {
        write_state_record(LOGGING_IDLE_OFF);
    }
}

bool key_event_log_is_on(void) {
    return logging.is_on;
}
