/** @file
 * The code that the checker files share: buffers and arrays that grow, the messages
 * of findings, the sort order of findings, and the output.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

/** The number of bytes that one read gets from a file. */
#define READ_CHUNK_SIZE 4096

/** The factor by which a full array grows. */
#define GROWTH_FACTOR 2

/** The number of items that an array gets in addition to twice the items in use. */
#define MIN_CAPACITY 16

/**
 * Makes space in an array for one or more items after the first @p count items.
 *
 * @param items     The array, or NULL if it has no allocation.
 * @param item_size The size of one item.
 * @param capacity  The number of items that are allocated. The function changes it
 *                  when the array grows.
 * @param count     The number of items in use.
 * @return The array. Its address can change when it grows.
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
 * Makes space in a buffer for @p extra more bytes and the terminator.
 *
 * @param buffer The buffer.
 * @param extra  The number of bytes to add after the current length.
 */
static void reserve_bytes(buffer_t *buffer, size_t extra) {
    buffer->data =
        reserve(buffer->data, sizeof(char), &buffer->capacity, buffer->length + extra);
}

/**
 * Adds a C string at the end of a buffer.
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
 * Adds all remaining bytes of a stream at the end of a buffer, without a terminator.
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
 * @param format  The printf() format of the message. The format arguments follow it.
 */
void describe(finding_t *finding, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(finding->message, sizeof(finding->message), format, arguments);
    va_end(arguments);
}

/**
 * Compares two numbers for qsort().
 *
 * @param left  The first number.
 * @param right The second number.
 * @return A negative value, zero or a positive value if @p left is less than, equal
 *         to or more than @p right.
 */
static int compare_numbers(size_t left, size_t right) {
    return (left > right) - (left < right);
}

/**
 * Compares two findings of one file for qsort(). The sort keys are the line, the
 * column and the report order.
 *
 * @param first  The first finding.
 * @param second The second finding.
 * @return A negative value, zero or a positive value if @p first comes before, at
 *         the same position as, or after @p second.
 */
int compare_findings(const void *first, const void *second) {
    const finding_t *one   = first;
    const finding_t *other = second;
    int              order = compare_numbers(one->line, other->line);
    order = order != 0 ? order : compare_numbers(one->column, other->column);
    return order != 0 ? order : compare_numbers(one->sequence, other->sequence);
}

/**
 * Prints each finding and the summary line.
 *
 * @param findings   The findings.
 * @param file_count The number of files that the checker examined.
 */
void print_findings(const findings_t *findings, int file_count) {
    for (size_t index = 0; index < findings->count; index++) {
        const finding_t *finding = &findings->items[index];
        printf("%s:%lu:%lu ", finding->path, finding->line, finding->column);
        printf("%s %s\n", finding->rule, finding->message);
    }
    printf("%zu findings in %d files\n", findings->count, file_count);
}
