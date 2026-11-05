/** @file
 * Support code of the checker: growing buffers and arrays, the messages of findings,
 * the order findings are printed in, and the output.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

/** Bytes read from a file at a time. */
#define READ_CHUNK_SIZE 4096

/** Factor a full array grows by. */
#define GROWTH_FACTOR 2

/** Items a growing array holds beyond twice those in use. */
#define MIN_CAPACITY 16

/**
 * Makes room in an array for at least one item after its first @p count.
 *
 * @param items     The array, or NULL if none is allocated yet.
 * @param item_size Size of one item.
 * @param capacity  Items allocated; updated when the array grows.
 * @param count     Items in use.
 * @return The array, moved if it grew.
 */
void *reserve(void *items, size_t item_size, size_t *capacity, size_t count) {
    if (count < *capacity) {
        return items;
    }
    *capacity   = count * GROWTH_FACTOR + MIN_CAPACITY;
    void *grown = realloc(items, *capacity * item_size);
    if (grown == NULL) {
        perror("check_standards");
        abort();
    }
    return grown;
}

/**
 * Makes room in a buffer for @p extra more bytes and the terminator.
 *
 * @param buffer The buffer.
 * @param extra  Bytes to add after its current length.
 */
static void reserve_bytes(buffer_t *buffer, size_t extra) {
    buffer->data =
        reserve(buffer->data, sizeof(char), &buffer->capacity, buffer->length + extra);
}

/**
 * Appends a C string to a buffer.
 *
 * @param buffer The buffer.
 * @param text   The string.
 */
void append(buffer_t *buffer, const char *text) {
    size_t length = strlen(text);
    reserve_bytes(buffer, length);
    memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
}

/**
 * Appends everything left in a stream to a buffer, without the terminator.
 *
 * @param buffer The buffer.
 * @param stream The stream.
 * @return 0, or the errno value of a read error.
 */
int append_stream(buffer_t *buffer, FILE *stream) {
    size_t read_size = 0;
    do {
        reserve_bytes(buffer, READ_CHUNK_SIZE);
        read_size =
            fread(buffer->data + buffer->length, sizeof(char), READ_CHUNK_SIZE, stream);
        buffer->length += read_size;
    } while (read_size == READ_CHUNK_SIZE);
    if (ferror(stream) != 0) {
        return errno;
    }
    return 0;
}

/**
 * Writes the message of a finding.
 *
 * @param finding The finding.
 * @param format  printf() format of the message, followed by its arguments.
 */
void describe(finding_t *finding, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(finding->message, sizeof(finding->message), format, arguments);
    va_end(arguments);
}

/**
 * Orders two numbers for qsort().
 *
 * @param left  The first number.
 * @param right The second number.
 * @return Negative, zero or positive as @p left is less than, equal to or greater
 *         than @p right.
 */
static int compare_numbers(size_t left, size_t right) {
    return (left > right) - (left < right);
}

/**
 * Orders two findings of one file by line, column and reporting order, for qsort().
 *
 * @param first  The first finding.
 * @param second The second finding.
 * @return Negative, zero or positive as @p first goes before, with or after
 *         @p second.
 */
int compare_findings(const void *first, const void *second) {
    const finding_t *one   = first;
    const finding_t *other = second;
    int              order = compare_numbers(one->line, other->line);
    order = order != 0 ? order : compare_numbers(one->column, other->column);
    return order != 0 ? order : compare_numbers(one->sequence, other->sequence);
}

/**
 * Prints every finding and the summary line.
 *
 * @param findings   The findings.
 * @param file_count Number of files checked.
 */
void print_findings(const findings_t *findings, int file_count) {
    for (size_t index = 0; index < findings->count; index++) {
        const finding_t *finding = &findings->items[index];
        printf("%s:%lu:%lu ", finding->path, finding->line, finding->column);
        printf("%s %s\n", finding->rule, finding->message);
    }
    printf("%zu findings in %d files\n", findings->count, file_count);
}
