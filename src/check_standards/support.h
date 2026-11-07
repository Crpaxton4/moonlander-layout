/** @file
 * Interface of support.c and the types that the checker files share
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

/// Bytes of a finding message, with its terminator. A longer message is cut
#define MESSAGE_SIZE 160

/// tree-sitter counts lines and columns from 0, but a finding counts from 1
#define FIRST_POSITION 1

/// Characters that indent a line or separate the words of a line
#define BLANKS " \t"

/// Number of items in an array. A pointer gives a wrong result
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/// append() keeps the bytes NUL-terminated, but append_stream() does not
typedef struct {
    char  *data;     ///< Bytes, or NULL before the first append
    size_t length;   ///< Bytes in use, without a terminator
    size_t capacity; ///< Bytes that data can hold
} buffer_t;

/// Rule violation at one position of a file
typedef struct {
    size_t        sequence; ///< Report order, the last sort key for a stable sort
    unsigned long line;     ///< Line from FIRST_POSITION
    unsigned long column;   ///< Column in bytes, not characters, from FIRST_POSITION
    const char   *path;     ///< File as the command line gives it
    const char   *rule;     ///< Rule name that the output shows
    char          message[MESSAGE_SIZE]; ///< Text that describe() writes
} finding_t;

/// Findings of each file are in sort order after the check of that file
typedef struct {
    finding_t *items;    ///< Findings, or NULL before the first finding
    size_t     count;    ///< Findings in use
    size_t     capacity; ///< Findings that items can hold
} findings_t;

/**
 * Grows an array so that it can hold one more item than count.
 *
 * The array can move, so use the return value. Without memory, the program stops.
 *
 * @param items     Array, or NULL for a new array.
 * @param item_size Bytes of one item.
 * @param capacity  Items that the array can hold. The function updates it.
 * @param count     Items in use.
 * @return Array that can hold more than count items.
 */
void *reserve(void *items, size_t item_size, size_t *capacity, size_t count);

/**
 * Adds a string to the end of a buffer and keeps the terminator.
 *
 * @param buffer Buffer that gets the text.
 * @param text   NUL-terminated text.
 */
void append(buffer_t *buffer, const char *text);

/**
 * Adds all bytes of a stream to the end of a buffer. It adds no terminator.
 *
 * @param buffer Buffer that gets the bytes.
 * @param stream Stream that the function reads to its end.
 * @return 0, or the errno value of a read error.
 */
int append_stream(buffer_t *buffer, FILE *stream);

/**
 * Writes the message of a finding as printf() does.
 *
 * @param finding Finding that gets the message.
 * @param format  printf() format of the message.
 * @param ...     Values for the format.
 */
void describe(finding_t *finding, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

/**
 * Orders findings by line, column and sequence, for qsort().
 *
 * @param first  First finding_t.
 * @param second Second finding_t.
 * @return Negative, 0 or positive, as qsort() requires.
 */
int compare_findings(const void *first, const void *second);

/**
 * Prints each finding on its own line, then a summary line.
 *
 * @param findings   Findings in output order.
 * @param file_count Files that the run checked.
 */
void print_findings(const findings_t *findings, int file_count);
