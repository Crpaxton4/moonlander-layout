/** @file
 * Logging state, idle time and report comparison of the key event log.
 */

#include "key_event_log/key_event_log_logic.h"

#include <string.h>

/** Key codes in one byte of a key bitmap */
#define KEYS_PER_BITMAP_BYTE 8

/** Bit of a key code in its byte of a key bitmap, as QMK's add_key_bit() sets it */
#define KEY_CODE_BIT(code) ((uint8_t)(1U << ((code) % KEYS_PER_BITMAP_BYTE)))

/** Text of each state, indexed by state */
static const char *const state_names[] = {
    [LOGGING_OFF]      = "off",
    [LOGGING_ON]       = "on",
    [LOGGING_IDLE_OFF] = "idle-off",
};

enum logging_state logging_toggle(logging_t *logging, uint32_t now) {
    enum logging_state state = logging->is_on ? LOGGING_OFF : LOGGING_ON;
    logging->is_on           = state == LOGGING_ON;
    logging->last_push_time  = now;
    return state;
}

void logging_note_push(logging_t *logging, uint32_t now) {
    logging->last_push_time = now;
}

bool logging_idle_ended(logging_t *logging, uint32_t now) {
    if (!logging->is_on || now - logging->last_push_time < LOGGING_IDLE_MILLISECONDS) {
        return false;
    }
    logging->is_on = false;
    return true;
}

bool logging_report_changed(logging_t *logging, const logged_report_t *report) {
    if (memcmp(&logging->last_report, report, sizeof(*report)) == 0) {
        return false;
    }
    logging->last_report = *report;
    return true;
}

void keys_from_bitmap(uint8_t *keys, const uint8_t *bits, size_t byte_count) {
    memset(keys, 0, LOGGED_REPORT_KEY_COUNT);
    size_t slot       = 0;
    size_t code_count = byte_count * KEYS_PER_BITMAP_BYTE;
    for (size_t code = 0; code < code_count && slot < LOGGED_REPORT_KEY_COUNT; code++) {
        if ((bits[code / KEYS_PER_BITMAP_BYTE] & KEY_CODE_BIT(code)) != 0) {
            keys[slot] = (uint8_t)code;
            slot++;
        }
    }
}

const char *logging_state_name(enum logging_state state) {
    return state_names[state];
}

char key_event_direction(bool pressed) {
    return pressed ? 'd' : 'u';
}
