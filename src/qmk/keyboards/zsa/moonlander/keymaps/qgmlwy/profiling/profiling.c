/** @file
 * Toggle key, measured calls and records of the profiling debug build.
 *
 * Measured calls nest. Thus the cycles of record_user also include the measure of
 * each record handler that it calls, about 100 cycles for each handler.
 */

#include QMK_KEYBOARD_H

#include "matrix.h"

#include "keycodes/keycodes.h"
#include "profiling/profiling.h"
#include "profiling/profiling_hardware.h"

/** Profiling state; all zero at power-up, so profiling is off */
static profile_t profile;

/**
 * Gives a value for a record. uint32_t is unsigned long on the keyboard and
 * unsigned int on a test host, so one printf() format needs a fixed type.
 *
 * @param value Value of a record field.
 * @return Same value.
 */
static unsigned long record_number(uint32_t value) {
    return value;
}

/**
 * Writes the clock record, which a converter needs to change cycles into time.
 *
 * @param now Time in ms.
 */
static void write_clock_record(uint32_t now) {
    uprintf("KL,C,%lu,%lu\n", record_number(now), record_number(core_clock_hz()));
}

/**
 * Writes one profile record for each measured call.
 *
 * @param now Time in ms.
 */
static void write_profile_records(uint32_t now) {
    for (size_t call = 0; call < PROFILED_CALL_COUNT; call++) {
        const call_statistics_t *statistics = &profile.statistics[call];
        uprintf("KL,P,%lu,%s,%lu,%lu,%lu\n", record_number(now),
                profiled_call_name(call), record_number(statistics->call_count),
                record_number(statistics->cycle_sum),
                record_number(statistics->cycle_max));
    }
}

/**
 * Writes the records of the print interval that ends now, then starts the next
 * interval.
 *
 * @param now Time in ms.
 */
static void write_interval_records(uint32_t now) {
    write_profile_records(now);
    uprintf("KL,R,%lu,%lu,%lu\n", record_number(now), record_number(profile.scan_count),
            record_number(profile_interval_length(&profile, now)));
    uprintf("KL,H,%lu,%lu\n", record_number(now),
            record_number(main_stack_free_bytes()));
    profile_start_interval(&profile, now);
}

/**
 * Changes profiling on a push of the toggle key.
 *
 * @param record Event of the toggle key.
 * @return False, so the toggle key sends no key.
 */
static bool toggle_profiling(const keyrecord_t *record) {
    if (!record->event.pressed) {
        return false;
    }
    uint32_t now = timer_read32();
    if (profile_toggle(&profile, now)) {
        start_cycle_counter();
        write_clock_record(now);
    }
    return false;
}

/**
 * Gives a key event to a handler, and measures that call while profiling is on.
 *
 * @param call    Measured call of the handler, a profiled_call value.
 * @param handler Key event handler.
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return Result of the handler.
 */
static bool measure_handler(size_t call, measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record) {
    if (!profile.is_on) {
        return handler(keycode, record);
    }
    uint32_t cycles = 0;
    bool     result = measure_handler_cycles(handler, keycode, record, &cycles);
    call_statistics_add(&profile.statistics[call], cycles);
    return result;
}

bool process_profiled_event(measured_handler_t dispatcher, uint16_t keycode,
                            keyrecord_t *record) {
    if (keycode == PROFILING_TOGGLE) {
        return toggle_profiling(record);
    }
    return measure_handler(PROFILED_RECORD_USER, dispatcher, keycode, record);
}

bool profile_record_handler(size_t index, measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record) {
    return measure_handler(FIRST_PROFILED_HANDLER + index, handler, keycode, record);
}

bool profiling_is_on(void) {
    return profile.is_on;
}

/**
 * Counts each matrix scan, and writes the records at the end of each print
 * interval. QMK calls this hook once for each scan of the key matrix, so the count
 * needs no QMK debug output.
 */
void matrix_scan_user(void) {
    if (!profile.is_on) {
        return;
    }
    profile_note_scan(&profile);
    uint32_t now = timer_read32();
    if (profile_interval_ended(&profile, now)) {
        write_interval_records(now);
    }
}
