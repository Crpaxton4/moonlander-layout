/** @file
 * Output file paths and the count files
 */

// #region Includes

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "outputs.h"
#include "report.h"
#include "support.h"

// #endregion

// #region Macros

/** Windows also accepts this separator */
#define PATH_SEPARATOR "/"

/** Percent as hundredths, so that the output has two decimals and no float */
#define HUNDREDTHS_PER_WHOLE 10000ULL

/** Hundredths in one percent */
#define HUNDREDTHS_PER_PERCENT 100ULL

/** Decimals of an average cycle count */
#define CYCLE_DECIMALS 2

/** First line of the profile file */
#define PROFILE_HEADER                                                   \
    "handler,calls,average cycles,maximum cycles,average us,maximum us," \
    "worst session,worst ms\n"

/** Half of the whole goes into the sum before the division, so that it rounds */
#define ROUNDING_DIVISOR 2ULL

// #endregion

// #region Types

/**
 * Writes the lines of one count file.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
typedef void count_writer_t(FILE *file, converter_t *converter);

/** Count file and the function that writes its lines */
typedef struct {
    const char     *name;   /**< File name in the output folder */
    count_writer_t *writer; /**< Writer of the lines */
} count_file_t;

// #endregion

// #region Function declarations

static void write_usage_lines(FILE *file, converter_t *converter);
static void write_correction_lines(FILE *file, converter_t *converter);
static void write_shortcut_lines(FILE *file, converter_t *converter);
static void write_profile_lines(FILE *file, converter_t *converter);

// #endregion

// #region Data

/** Count files in the order of their writes. The report uses the sorted tables */
static const count_file_t count_files[] = {
    {OUTPUT_USAGE, write_usage_lines},
    {OUTPUT_CORRECTIONS, write_correction_lines},
    {OUTPUT_SHORTCUTS, write_shortcut_lines},
    {OUTPUT_PROFILE, write_profile_lines},
    {OUTPUT_REPORT, write_report},
};

// #endregion

// #region Static functions

/**
 * Writes the push count of each position and layer. Each layer up to the highest
 * layer of the pushes lists all matrix positions.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_usage_lines(FILE *file, converter_t *converter) {
    for (unsigned layer = 0; layer <= converter->highest_layer; layer++) {
        add_matrix_positions(&converter->usage, (uint8_t)layer);
    }
    sort_position_counts(&converter->usage);
    fputs("row,col,layer,pushes\n", file);
    for (size_t index = 0; index < converter->usage.count; index++) {
        const position_count_t *item = &converter->usage.items[index];
        fprintf(file, "%u,%u,%u,%llu\n", (unsigned)item->key.position.row,
                (unsigned)item->key.position.col, (unsigned)item->key.layer,
                item->count);
    }
}

/**
 * Gives a part as a percent of a whole, rounded to hundredths.
 *
 * @param part  Counted part.
 * @param whole Total, 0 gives 0.
 * @return Percent in hundredths.
 */
static count_t percent_hundredths(count_t part, count_t whole) {
    if (whole == 0) {
        return 0;
    }
    return (part * HUNDREDTHS_PER_WHOLE + whole / ROUNDING_DIVISOR) / whole;
}

/**
 * Writes the correction totals, then the Backspace count of each position.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_correction_lines(FILE *file, converter_t *converter) {
    count_t percent =
        percent_hundredths(converter->correction_pushes, converter->character_pushes);
    add_matrix_positions(&converter->corrections, CORRECTION_LAYER);
    sort_position_counts(&converter->corrections);
    fprintf(file, "metric,value\nbackspace and delete pushes,%llu\n",
            converter->correction_pushes);
    fprintf(file, "character pushes,%llu\n", converter->character_pushes);
    fprintf(file, "correction percent,%llu.%02llu\n", percent / HUNDREDTHS_PER_PERCENT,
            percent % HUNDREDTHS_PER_PERCENT);
    fputs("row,col,backspace pushes less than 1 s after a push\n", file);
    for (size_t index = 0; index < converter->corrections.count; index++) {
        const position_count_t *item = &converter->corrections.items[index];
        fprintf(file, "%u,%u,%llu\n", (unsigned)item->key.position.row,
                (unsigned)item->key.position.col, item->count);
    }
}

/**
 * Writes the count of each shortcut.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_shortcut_lines(FILE *file, converter_t *converter) {
    sort_named_counts(&converter->shortcuts);
    fputs("shortcut,count\n", file);
    for (size_t index = 0; index < converter->shortcuts.count; index++) {
        const named_count_t *item = &converter->shortcuts.items[index];
        fprintf(file, "%s,%llu\n", item->name, item->count);
    }
}

/**
 * Writes the line of one handler.
 *
 * @param file    Open output file.
 * @param profile Profiling totals with the clock of the run.
 * @param index   Index of the handler in the handler table.
 */
static void write_handler_line(FILE *file, const profile_t *profile, size_t index) {
    const handler_totals_t *handler = &profile->handlers.items[index];
    char                    average_cycles[NUMBER_TEXT_SIZE];
    char                    average_time[NUMBER_TEXT_SIZE];
    char                    maximum_time[NUMBER_TEXT_SIZE];
    ratio_t cycles = {.part = handler->cycle_sum, .whole = handler->calls};
    format_ratio(average_cycles, cycles, CYCLE_DECIMALS);
    format_microseconds(average_time, average_call_time(profile, handler));
    format_microseconds(maximum_time, slowest_call_time(profile, handler));
    fprintf(file, "%s,%llu,%s,%llu,%s,%s,%zu,%lu\n", handler->name, handler->calls,
            average_cycles, handler->cycle_max, average_time, maximum_time,
            handler->worst_session, (unsigned long)handler->worst_time);
}

/**
 * Writes the totals of each handler, in the order of the first handler records.
 *
 * @param file      Open output file.
 * @param converter Totals of the run.
 */
static void write_profile_lines(FILE *file, converter_t *converter) {
    const profile_t       *profile  = &converter->profile;
    const handler_table_t *handlers = &profile->handlers;
    fputs(PROFILE_HEADER, file);
    for (size_t index = 0; index < handlers->count; index++) {
        write_handler_line(file, profile, index);
    }
}

// #endregion

// #region Public functions

void report_file_error(const char *path) {
    fprintf(stderr, "key_log_converter: %s: %s\n", path, strerror(errno));
}

FILE *open_output(const char *folder, const char *name) {
    size_t size = strlen(folder) + strlen(PATH_SEPARATOR) + strlen(name) + sizeof("");
    char  *path = malloc(size);
    if (path == NULL) {
        perror("key_log_converter");
        abort();
    }
    snprintf(path, size, "%s%s%s", folder, PATH_SEPARATOR, name);
    FILE *file = fopen(path, "wb");
    if (file == NULL) {
        report_file_error(path);
    }
    free(path);
    return file;
}

bool close_output(FILE *file, const char *name) {
    if (file == NULL) {
        return true;
    }
    bool written = ferror(file) == 0;
    if (fclose(file) != 0 || !written) {
        report_file_error(name);
        return false;
    }
    return true;
}

bool write_count_files(converter_t *converter, const char *folder) {
    bool success = true;
    for (size_t index = 0; index < COUNT_OF(count_files); index++) {
        FILE *file = open_output(folder, count_files[index].name);
        if (file == NULL) {
            return false;
        }
        count_files[index].writer(file, converter);
        if (!close_output(file, count_files[index].name)) {
            success = false;
        }
    }
    return success;
}

// #endregion
