/** @file
 * Statistics, print interval and free stack count of the profiling debug build.
 */

// #region Includes

#include "profiling/profiling_logic.h"

#include <string.h>

// #endregion

// #region Data

/** Name of each measured call, as the record contract gives it */
static const char *const profiled_call_names[] = {
    [PROFILED_RECORD_USER]   = "record_user",
    [PROFILED_KEY_LOG]       = "key_log",
    [PROFILED_PLAIN_MODE]    = "plain_mode",
    [PROFILED_CASE_MODES]    = "case_modes",
    [PROFILED_BRACKET_PAIRS] = "bracket_pairs",
    [PROFILED_SPECIAL_MODE]  = "special_mode",
    [PROFILED_SYMBOL_HOLDS]  = "symbol_holds",
    [PROFILED_PRECISION_KEY] = "precision_key",
};

// #endregion

// #region Public functions

bool profile_toggle(profile_t *profile, uint32_t now) {
    if (profile->is_on) {
        profile->is_on = false;
        return false;
    }
    profile->is_on = true;
    profile_start_interval(profile, now);
    return true;
}

void call_statistics_add(call_statistics_t *statistics, uint32_t cycles) {
    statistics->call_count++;
    statistics->cycle_sum += cycles;
    if (cycles > statistics->cycle_max) {
        statistics->cycle_max = cycles;
    }
}

void profile_note_scan(profile_t *profile) {
    profile->scan_count++;
}

uint32_t profile_interval_length(const profile_t *profile, uint32_t now) {
    return now - profile->interval_start;
}

bool profile_interval_ended(const profile_t *profile, uint32_t now) {
    if (!profile->is_on) {
        return false;
    }
    return profile_interval_length(profile, now) >= PROFILING_INTERVAL_MILLISECONDS;
}

void profile_start_interval(profile_t *profile, uint32_t now) {
    memset(profile->statistics, 0, sizeof(profile->statistics));
    profile->scan_count     = 0;
    profile->interval_start = now;
}

const char *profiled_call_name(size_t call) {
    return profiled_call_names[call];
}

uint32_t free_stack_bytes(const uint32_t *stack_bottom, size_t word_count) {
    size_t free_words = 0;
    while (free_words < word_count && stack_bottom[free_words] == STACK_FILL_WORD) {
        free_words++;
    }
    return (uint32_t)(free_words * sizeof(*stack_bottom));
}

// #endregion
