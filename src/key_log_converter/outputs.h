/** @file
 * Output files of the converter in the output folder
 */

#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "converter.h"

/** Typed text, for carpalx and the keyboard_layout_optimizer ngrams tool */
#define OUTPUT_CORPUS "corpus.txt"

/** keycode,row,col,layer line of each push, for the QMK heatmap tool */
#define OUTPUT_HEATMAP "heatmap.csv"

/** Pushes of each position and layer */
#define OUTPUT_USAGE "usage.csv"

/** Correction percent and the Backspace pushes after each position */
#define OUTPUT_CORRECTIONS "corrections.csv"

/** Pushes of each shortcut */
#define OUTPUT_SHORTCUTS "shortcuts.csv"

/** Calls and times of each record handler */
#define OUTPUT_PROFILE "profile.csv"

/** Markdown summary of the usage, the profiling and the data quality */
#define OUTPUT_REPORT "report.md"

/**
 * Writes the errno message of a failed file operation to standard error.
 *
 * @param path File of the operation.
 */
void report_file_error(const char *path);

/**
 * Opens an output file for writing. Binary mode keeps LF line breaks on Windows.
 *
 * @param folder Output folder.
 * @param name   File name in the folder.
 * @return File, or NULL after an error message.
 */
FILE *open_output(const char *folder, const char *name);

/**
 * Closes an output file and checks for write errors.
 *
 * @param file File, or NULL for a file that did not open.
 * @param name File name for the error message.
 * @return false after a write error.
 */
bool close_output(FILE *file, const char *name);

/**
 * Writes the usage, correction, shortcut and profile files, then the report. Each
 * table gets its zero counts and its sort order first.
 *
 * @param converter Totals of the run.
 * @param folder    Output folder.
 * @return false after an error message.
 */
bool write_count_files(converter_t *converter, const char *folder);
