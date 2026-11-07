/** @file
 * Profiling totals with a linear search. The firmware has few handlers
 */

#include <stdio.h>
#include <string.h>

#include "profile.h"

/**
 * Finds the totals of the handler of a record, and adds zero totals if none
 * exist.
 *
 * @param table  Handler totals.
 * @param record Handler record.
 * @return Totals in the table. The next add can move them.
 */
static handler_totals_t *find_handler(handler_table_t *table, const record_t *record) {
    const char *name = record->handler.name;
    for (size_t index = 0; index < table->count; index++) {
        if (strcmp(table->items[index].name, name) == 0) {
            return &table->items[index];
        }
    }
    table->items =
        reserve(table->items, sizeof(handler_totals_t), &table->capacity, table->count);
    handler_totals_t *item = &table->items[table->count++];
    *item                  = (handler_totals_t){.calls = 0};
    snprintf(item->name, sizeof(item->name), "%s", name);
    return item;
}

void add_handler_interval(profile_t *profile, const record_t *record, size_t session) {
    const handler_interval_t *interval       = &record->handler;
    handler_totals_t         *totals         = find_handler(&profile->handlers, record);
    bool                      first_interval = totals->worst_session == 0;
    totals->calls += interval->calls;
    totals->cycle_sum += interval->cycle_sum;
    // With equal cycle_max values, the earliest interval stays the worst interval
    if (first_interval || interval->cycle_max > totals->cycle_max) {
        totals->cycle_max     = interval->cycle_max;
        totals->worst_session = session;
        totals->worst_time    = record->time;
    }
}

void add_scan_interval(profile_t *profile, const scan_interval_t *scans, bool typing) {
    scan_totals_t *totals = typing ? &profile->typing_scans : &profile->idle_scans;
    totals->scan_count += scans->scan_count;
    totals->interval_ms += scans->interval_ms;
}

void add_stack_record(profile_t *profile, uint32_t free_bytes) {
    if (!profile->has_stack || free_bytes < profile->lowest_free_stack) {
        profile->lowest_free_stack = free_bytes;
    }
    profile->has_stack = true;
}

scan_totals_t all_scans(const profile_t *profile) {
    const scan_totals_t *idle   = &profile->idle_scans;
    const scan_totals_t *typing = &profile->typing_scans;
    return (scan_totals_t){
        .scan_count  = idle->scan_count + typing->scan_count,
        .interval_ms = idle->interval_ms + typing->interval_ms,
    };
}

ratio_t average_scan_time(const profile_t *profile) {
    scan_totals_t scans = all_scans(profile);
    return (ratio_t){
        .part  = scans.interval_ms,
        .whole = scans.scan_count * MILLISECONDS_PER_SECOND,
    };
}

// A clock of NO_CLOCK gives a whole of 0, thus no value
ratio_t average_call_time(const profile_t *profile, const handler_totals_t *handler) {
    return (ratio_t){
        .part  = handler->cycle_sum,
        .whole = handler->calls * profile->cpu_hz,
    };
}

ratio_t slowest_call_time(const profile_t *profile, const handler_totals_t *handler) {
    return (ratio_t){.part = handler->cycle_max, .whole = profile->cpu_hz};
}
