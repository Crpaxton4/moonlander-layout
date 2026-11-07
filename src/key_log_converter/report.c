/** @file
 * Report sections. Each value is an integer ratio with fixed decimals, and each
 * value that the logs cannot give is NO_VALUE_TEXT
 */

// #region Includes

#include <stdio.h>
#include <string.h>

#include "markdown.h"
#include "report.h"
#include "support.h"

// #endregion

// #region Macros

/** Unit of a time in microseconds */
#define MICROSECONDS_UNIT "µs"

/** Rows of each top list */
#define TOP_LIST_LENGTH 10

/** Milliseconds in one minute */
#define MILLISECONDS_PER_MINUTE 60000ULL

/** Seconds in one minute */
#define SECONDS_PER_MINUTE 60ULL

/** Minutes in one hour */
#define MINUTES_PER_HOUR 60ULL

/** Seconds in one hour */
#define SECONDS_PER_HOUR 3600ULL

/** Keys of the correction rate */
#define KEYS_PER_CORRECTION_RATE 100ULL

/** Percent in one whole */
#define PERCENT_PER_WHOLE 100ULL

/** Decimals of a rate per minute or per second */
#define RATE_DECIMALS 1

/** Decimals of a correction rate or a percent */
#define PERCENT_DECIMALS 2

// #endregion

// #region Enumerations

/** Columns of a metric table */
enum {
    METRIC_LABEL_COLUMN, /**< Name of the metric */
    METRIC_VALUE_COLUMN, /**< Value and unit */
    METRIC_COLUMN_COUNT, /**< Columns */
};

/** Columns of a top list */
enum {
    TOP_NAME_COLUMN,   /**< Name */
    TOP_PUSHES_COLUMN, /**< Count */
    TOP_COLUMN_COUNT,  /**< Columns */
};

/** Columns of the handler table */
enum {
    HANDLER_NAME_COLUMN,    /**< Handler name */
    HANDLER_CALLS_COLUMN,   /**< Calls */
    HANDLER_AVERAGE_COLUMN, /**< Average time of one call */
    HANDLER_MAXIMUM_COLUMN, /**< Time of the slowest call */
    HANDLER_SHARE_COLUMN,   /**< Average call as a percent of the average scan */
    HANDLER_COLUMN_COUNT,   /**< Columns */
};

// #endregion

// #region Types

/** Label and value of a metric row with a count */
typedef struct {
    const char *label; /**< Name of the metric */
    count_t     value; /**< Count */
} count_metric_t;

/** Heading and columns of a list of the largest name counts */
typedef struct {
    const char *heading;                   /**< Markdown heading */
    column_t    columns[TOP_COLUMN_COUNT]; /**< Columns */
    const char *name_mark;                 /**< Text on the two sides of each name */
} top_list_t;

// #endregion

// #region Data

/** Columns of a metric table */
static const column_t metric_columns[METRIC_COLUMN_COUNT] = {
    [METRIC_LABEL_COLUMN] = {"Metric", ALIGN_LEFT},
    [METRIC_VALUE_COLUMN] = {"Value", ALIGN_RIGHT},
};

/** Columns of the handler table */
static const column_t handler_columns[HANDLER_COLUMN_COUNT] = {
    [HANDLER_NAME_COLUMN]    = {"Handler", ALIGN_LEFT},
    [HANDLER_CALLS_COLUMN]   = {"Calls", ALIGN_RIGHT},
    [HANDLER_AVERAGE_COLUMN] = {"Average µs", ALIGN_RIGHT},
    [HANDLER_MAXIMUM_COLUMN] = {"Maximum µs", ALIGN_RIGHT},
    [HANDLER_SHARE_COLUMN]   = {"Share of a scan", ALIGN_RIGHT},
};

/** Names of the new keys */
static const top_list_t key_list = {
    .heading   = "### Top keys",
    .columns   = {{"Key", ALIGN_LEFT}, {"Pushes", ALIGN_RIGHT}},
    .name_mark = "",
};

/** Heatmap lines, as code because they have commas */
static const top_list_t heatmap_list = {
    .heading   = "### Busiest heatmap rows",
    .columns   = {{"Heatmap row", ALIGN_LEFT}, {"Pushes", ALIGN_RIGHT}},
    .name_mark = "`",
};

/** Shortcut names */
static const top_list_t shortcut_list = {
    .heading   = "### Top shortcuts",
    .columns   = {{"Shortcut", ALIGN_LEFT}, {"Pushes", ALIGN_RIGHT}},
    .name_mark = "",
};

// #endregion

// #region Static functions

/**
 * Writes a duration as hours, minutes and seconds. The seconds round down.
 *
 * @param text         Buffer that gets the duration.
 * @param milliseconds Duration.
 */
static void format_duration(char text[NUMBER_TEXT_SIZE], count_t milliseconds) {
    count_t total_seconds = milliseconds / MILLISECONDS_PER_SECOND;
    count_t hours         = total_seconds / SECONDS_PER_HOUR;
    count_t minutes       = total_seconds / SECONDS_PER_MINUTE % MINUTES_PER_HOUR;
    count_t seconds       = total_seconds % SECONDS_PER_MINUTE;
    snprintf(text, NUMBER_TEXT_SIZE, "%llu:%02llu:%02llu", hours, minutes, seconds);
}

/**
 * Adds a unit after a value. NO_VALUE_TEXT gets no unit.
 *
 * @param text Buffer of NUMBER_TEXT_SIZE bytes with a value.
 * @param unit Unit.
 */
static void append_unit(char text[NUMBER_TEXT_SIZE], const char *unit) {
    size_t length = strlen(text);
    if (strcmp(text, NO_VALUE_TEXT) == 0) {
        return;
    }
    snprintf(text + length, NUMBER_TEXT_SIZE - length, " %s", unit);
}

/**
 * Adds a row to a metric table.
 *
 * @param table Metric table.
 * @param label Name of the metric.
 * @return Value cell of CELL_SIZE bytes, empty. The next add can move it.
 */
static char *add_metric(table_t *table, const char *label) {
    table_row_t *row = add_table_row(table);
    snprintf(row->cells[METRIC_LABEL_COLUMN], CELL_SIZE, "%s", label);
    return row->cells[METRIC_VALUE_COLUMN];
}

/**
 * Adds a row with a count to a metric table.
 *
 * @param table Metric table.
 * @param label Name of the metric.
 * @param value Count.
 */
static void add_count_metric(table_t *table, const char *label, count_t value) {
    snprintf(add_metric(table, label), CELL_SIZE, "%llu", value);
}

/**
 * Writes the totals of the usage.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_usage_metrics(FILE *file, const converter_t *converter) {
    table_t table = {.columns = metric_columns, .column_count = METRIC_COLUMN_COUNT};
    ratio_t keys_per_minute = {
        .part  = converter->key_pushes * MILLISECONDS_PER_MINUTE,
        .whole = converter->logged_ms,
    };
    ratio_t correction_rate = {
        .part  = converter->correction_pushes * KEYS_PER_CORRECTION_RATE,
        .whole = converter->character_pushes,
    };
    add_count_metric(&table, "Files", converter->summary.files);
    add_count_metric(&table, "Sessions", converter->summary.sessions);
    format_duration(add_metric(&table, "Logged time"), converter->logged_ms);
    add_count_metric(&table, "Key pushes", converter->key_pushes);
    format_ratio(add_metric(&table, "Keys per minute"), keys_per_minute, RATE_DECIMALS);
    char *corrections = add_metric(&table, "Corrections per 100 keys");
    format_ratio(corrections, correction_rate, PERCENT_DECIMALS);
    write_table(file, &table);
}

/**
 * Writes the largest counts of a table of names. Equal counts are in name order.
 *
 * @param file   Open output file.
 * @param counts Table of names. It gets its sort order.
 * @param list   Heading and columns.
 */
static void write_top_list(FILE *file, named_counts_t *counts, const top_list_t *list) {
    table_t     table = {.columns = list->columns, .column_count = TOP_COLUMN_COUNT};
    const char *mark  = list->name_mark;
    sort_named_counts(counts);
    write_block(file, list->heading);
    for (size_t index = 0; index < counts->count && index < TOP_LIST_LENGTH; index++) {
        const named_count_t *item = &counts->items[index];
        table_row_t         *row  = add_table_row(&table);
        char                *name = row->cells[TOP_NAME_COLUMN];
        snprintf(name, CELL_SIZE, "%s%s%s", mark, item->name, mark);
        snprintf(row->cells[TOP_PUSHES_COLUMN], CELL_SIZE, "%llu", item->count);
    }
    write_table(file, &table);
}

/**
 * Writes the usage section.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_usage(FILE *file, converter_t *converter) {
    write_block(file, "## Usage");
    write_usage_metrics(file, converter);
    write_top_list(file, &converter->keys, &key_list);
    write_top_list(file, &converter->heatmap_rows, &heatmap_list);
    write_top_list(file, &converter->shortcuts, &shortcut_list);
}

/**
 * Writes the average call of a handler as a percent of the average scan. Both
 * times are whole nanoseconds first, so the product cannot overflow.
 *
 * @param text         Buffer of NUMBER_TEXT_SIZE bytes that gets the percent, or
 *                     NO_VALUE_TEXT.
 * @param call_seconds Average call time.
 * @param scan_seconds Average scan time.
 */
static void format_scan_share(char *text, ratio_t call_seconds, ratio_t scan_seconds) {
    if (call_seconds.whole == 0 || scan_seconds.whole == 0) {
        snprintf(text, NUMBER_TEXT_SIZE, "%s", NO_VALUE_TEXT);
        return;
    }
    count_t call_time = scaled_quotient(call_seconds, SECOND_TO_NANOSECOND_DECIMALS);
    ratio_t share     = {
        .part  = call_time * PERCENT_PER_WHOLE,
        .whole = scaled_quotient(scan_seconds, SECOND_TO_NANOSECOND_DECIMALS),
    };
    format_ratio(text, share, PERCENT_DECIMALS);
    append_unit(text, "%");
}

/**
 * Adds the row of one handler.
 *
 * @param table   Handler table.
 * @param profile Profiling totals.
 * @param handler Handler totals.
 */
static void add_handler_row(table_t *table, const profile_t *profile,
                            const handler_totals_t *handler) {
    table_row_t *row       = add_table_row(table);
    ratio_t      call_time = average_call_time(profile, handler);
    ratio_t      scan_time = average_scan_time(profile);
    snprintf(row->cells[HANDLER_NAME_COLUMN], CELL_SIZE, "`%s`", handler->name);
    snprintf(row->cells[HANDLER_CALLS_COLUMN], CELL_SIZE, "%llu", handler->calls);
    format_microseconds(row->cells[HANDLER_AVERAGE_COLUMN], call_time);
    format_microseconds(row->cells[HANDLER_MAXIMUM_COLUMN],
                        slowest_call_time(profile, handler));
    format_scan_share(row->cells[HANDLER_SHARE_COLUMN], call_time, scan_time);
}

/**
 * Writes the handler table, in the order of the first handler records.
 *
 * @param file    Open output file.
 * @param profile Profiling totals.
 */
static void write_handlers(FILE *file, const profile_t *profile) {
    table_t table = {.columns = handler_columns, .column_count = HANDLER_COLUMN_COUNT};
    write_block(file, "### Handlers");
    for (size_t index = 0; index < profile->handlers.count; index++) {
        add_handler_row(&table, profile, &profile->handlers.items[index]);
    }
    write_table(file, &table);
}

/**
 * Writes a scan rate.
 *
 * @param text  Buffer that gets scans per second.
 * @param scans Scans of a group of intervals.
 */
static void format_scan_rate(char text[NUMBER_TEXT_SIZE], const scan_totals_t *scans) {
    ratio_t rate = {
        .part  = scans->scan_count * MILLISECONDS_PER_SECOND,
        .whole = scans->interval_ms,
    };
    format_ratio(text, rate, RATE_DECIMALS);
    append_unit(text, "scans/s");
}

/**
 * Writes the lowest free stack.
 *
 * @param text    Buffer that gets bytes.
 * @param profile Profiling totals.
 */
static void format_stack(char text[NUMBER_TEXT_SIZE], const profile_t *profile) {
    unsigned long free_bytes = profile->lowest_free_stack;
    snprintf(text, NUMBER_TEXT_SIZE, "%s", NO_VALUE_TEXT);
    if (profile->has_stack) {
        snprintf(text, NUMBER_TEXT_SIZE, "%lu bytes", free_bytes);
    }
}

/**
 * Writes the core clock of the run.
 *
 * @param text    Buffer that gets Hz.
 * @param profile Profiling totals.
 */
static void format_clock(char text[NUMBER_TEXT_SIZE], const profile_t *profile) {
    snprintf(text, NUMBER_TEXT_SIZE, "%s", NO_VALUE_TEXT);
    if (profile->cpu_hz != NO_CLOCK) {
        snprintf(text, NUMBER_TEXT_SIZE, "%lu Hz", (unsigned long)profile->cpu_hz);
    }
}

/**
 * Writes the clock, scan and stack metrics.
 *
 * @param file    Open output file.
 * @param profile Profiling totals.
 */
static void write_scan_metrics(FILE *file, const profile_t *profile) {
    table_t table = {.columns = metric_columns, .column_count = METRIC_COLUMN_COUNT};
    write_block(file, "### Scans and stack");
    format_clock(add_metric(&table, "Core clock"), profile);
    char *scan_time = add_metric(&table, "Scan time");
    format_microseconds(scan_time, average_scan_time(profile));
    append_unit(scan_time, MICROSECONDS_UNIT);
    format_scan_rate(add_metric(&table, "Scan rate while idle"), &profile->idle_scans);
    char *typing_rate = add_metric(&table, "Scan rate while typing");
    format_scan_rate(typing_rate, &profile->typing_scans);
    format_stack(add_metric(&table, "Lowest free stack"), profile);
    write_table(file, &table);
}

/**
 * Writes the profiling section, or one line if the sessions have no profiling
 * record.
 *
 * @param file    Open output file.
 * @param profile Profiling totals.
 */
static void write_profiling(FILE *file, const profile_t *profile) {
    write_block(file, "## Profiling");
    if (profile->records == 0) {
        write_block(file, "No profiling records.");
        return;
    }
    write_handlers(file, profile);
    write_scan_metrics(file, profile);
}

/**
 * Writes the data quality section.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_data_quality(FILE *file, const converter_t *converter) {
    table_t table = {.columns = metric_columns, .column_count = METRIC_COLUMN_COUNT};
    const summary_t     *summary  = &converter->summary;
    const profile_t     *profile  = &converter->profile;
    const count_metric_t counts[] = {
        {"Ignored lines", summary->ignored_lines},
        {"Records outside sessions", summary->records_outside_sessions},
        {"Handler records with no clock record", profile->records_without_clock},
    };
    count_t profiling_ms = all_scans(profile).interval_ms;
    write_block(file, "## Data quality");
    for (size_t index = 0; index < COUNT_OF(counts); index++) {
        add_count_metric(&table, counts[index].label, counts[index].value);
    }
    format_duration(add_metric(&table, "Time with profiling on"), profiling_ms);
    write_table(file, &table);
}

// #endregion

// #region Public functions

void write_report(FILE *file, converter_t *converter) {
    fputs("# Key log report\n", file);
    write_usage(file, converter);
    write_profiling(file, &converter->profile);
    write_data_quality(file, converter);
}

// #endregion
