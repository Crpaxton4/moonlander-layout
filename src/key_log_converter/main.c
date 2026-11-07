/** @file
 * Key log converter entry point: log files in, corpus and metrics out
 */

// #region Includes

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "converter.h"
#include "outputs.h"
#include "record.h"

// #endregion

// #region Macros

/** argv index of the output folder. argv[0] is the program name */
#define OUTPUT_FOLDER_ARGUMENT 1

/** argv index of the first log file */
#define FIRST_LOG_ARGUMENT 2

/** Exit status of a wrong command line */
#define EXIT_USAGE 2

/** Bytes of a line buffer. A record is shorter, so a longer line is no record */
#define LINE_SIZE 256

/** Command line help */
#define USAGE_TEXT "usage: key_log_converter OUTPUT_FOLDER LOG_FILE...\n"

// #endregion

// #region Enumerations

/** Results of a line read */
typedef enum {
    LINE_COMPLETE,   /**< Line with a line break */
    LINE_INCOMPLETE, /**< Line that is too long or has no line break at the end */
    LINE_END,        /**< No more lines */
} line_status_t;

// #endregion

// #region Static functions

/**
 * Reads and discards the rest of a line that is too long for the buffer.
 *
 * @param file Log file.
 */
static void skip_to_line_end(FILE *file) {
    int character = 0;
    do {
        character = fgetc(file);
    } while (character != '\n' && character != EOF);
}

/**
 * Reads one line and removes its LF or CR LF line break. A line with no line
 * break at the end of a file can be cut, so it is incomplete.
 *
 * @param file Log file.
 * @param line Buffer of LINE_SIZE bytes.
 * @return Line status.
 */
static line_status_t read_line(FILE *file, char line[LINE_SIZE]) {
    // A read after the end or after an error has no defined result
    if (feof(file) != 0 || ferror(file) != 0 || fgets(line, LINE_SIZE, file) == NULL) {
        return LINE_END;
    }
    char *line_break = strchr(line, '\n');
    if (line_break == NULL) {
        skip_to_line_end(file);
        return LINE_INCOMPLETE;
    }
    *line_break = '\0';
    // hid_listen on Windows writes CR LF. Only CR characters at the end go
    char *carriage_return = strchr(line, '\r');
    if (carriage_return != NULL &&
        carriage_return + strspn(carriage_return, "\r") == line_break) {
        *carriage_return = '\0';
    }
    return LINE_COMPLETE;
}

/**
 * Converts one line, or counts it as ignored.
 *
 * @param converter Outputs and totals.
 * @param line      Line with no line break.
 * @param status    Line status from read_line().
 */
static void convert_line(converter_t *converter, char *line, line_status_t status) {
    record_t record;
    if (status != LINE_COMPLETE || !parse_record(line, &record)) {
        converter->summary.ignored_lines++;
        return;
    }
    convert_record(converter, &record);
}

/**
 * Converts all lines of one log file.
 *
 * @param converter Outputs and totals.
 * @param path      Log file.
 * @return false after an error message.
 */
static bool convert_file(converter_t *converter, const char *path) {
    FILE *file = fopen(path, "rb");
    if (file == NULL) {
        report_file_error(path);
        return false;
    }
    begin_stream(converter);
    char          line[LINE_SIZE];
    line_status_t status = read_line(file, line);
    while (status != LINE_END) {
        convert_line(converter, line, status);
        status = read_line(file, line);
    }
    end_stream(converter);
    bool success = ferror(file) == 0;
    fclose(file);
    if (!success) {
        report_file_error(path);
    }
    return success;
}

/**
 * Prints the counts of the run.
 *
 * @param summary Counts.
 */
static void print_summary(const summary_t *summary) {
    printf("files: %zu\n", summary->files);
    printf("sessions: %zu\n", summary->sessions);
    printf("records: %zu\n", summary->records);
    printf("records outside sessions: %zu\n", summary->records_outside_sessions);
    printf("ignored lines: %zu\n", summary->ignored_lines);
}

/**
 * Closes the corpus and heatmap files. Both close also after an error.
 *
 * @param converter Outputs of the run.
 * @return false after a write error.
 */
static bool close_streams(converter_t *converter) {
    bool corpus_closed  = close_output(converter->corpus, OUTPUT_CORPUS);
    bool heatmap_closed = close_output(converter->heatmap, OUTPUT_HEATMAP);
    if (!corpus_closed) {
        return false;
    }
    return heatmap_closed;
}

/**
 * Frees the count tables.
 *
 * @param converter Totals of the run.
 */
static void free_tables(converter_t *converter) {
    free(converter->usage.items);
    free(converter->corrections.items);
    free(converter->recent_pushes.items);
    free(converter->shortcuts.items);
    free(converter->keys.items);
    free(converter->heatmap_rows.items);
    free(converter->profile.handlers.items);
}

// #endregion

// #region Public functions

/**
 * Converts all log files into the output files.
 *
 * @param argc Number of arguments.
 * @param argv Program name, the output folder, then the log files.
 * @return EXIT_SUCCESS, EXIT_FAILURE after an error or EXIT_USAGE.
 */
int main(int argc, char *argv[]) {
    if (argc <= FIRST_LOG_ARGUMENT) {
        fputs(USAGE_TEXT, stderr);
        return EXIT_USAGE;
    }
    const char *folder    = argv[OUTPUT_FOLDER_ARGUMENT];
    converter_t converter = {.corpus = open_output(folder, OUTPUT_CORPUS)};
    converter.heatmap     = open_output(folder, OUTPUT_HEATMAP);
    bool success          = converter.corpus != NULL;
    if (converter.heatmap == NULL) {
        success = false;
    }
    for (int index = FIRST_LOG_ARGUMENT; success && index < argc; index++) {
        success = convert_file(&converter, argv[index]);
    }
    if (!close_streams(&converter) || !success) {
        success = false;
    } else {
        success = write_count_files(&converter, folder);
    }
    print_summary(&converter.summary);
    free_tables(&converter);
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

// #endregion
