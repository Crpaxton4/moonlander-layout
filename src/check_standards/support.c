/** @file
 * Buffers, findings and output of the checker
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

/// Bytes that each fread() call asks for
#define READ_CHUNK_SIZE 4096

/// Capacity multiplier for a full array, so that appends stay fast on average
#define GROWTH_FACTOR 2

/// Items added to each new capacity, so that small arrays do not grow often
#define MIN_CAPACITY 16

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
 * Makes space for extra bytes and also for the terminator.
 *
 * @param buffer Buffer that grows.
 * @param extra  Bytes after the current length.
 */
static void reserve_bytes(buffer_t *buffer, size_t extra) {
    buffer->data =
        reserve(buffer->data, sizeof(char), &buffer->capacity, buffer->length + extra);
}

void append(buffer_t *buffer, const char *text) {
    size_t length = strlen(text);
    reserve_bytes(buffer, length);
    memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
}

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

void describe(finding_t *finding, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(finding->message, sizeof(finding->message), format, arguments);
    va_end(arguments);
}

/**
 * Compares two numbers. A subtraction of size_t values can wrap, but this form
 * cannot.
 *
 * @param left  First number.
 * @param right Second number.
 * @return -1, 0 or 1.
 */
static int compare_numbers(size_t left, size_t right) {
    return (left > right) - (left < right);
}

int compare_findings(const void *first, const void *second) {
    const finding_t *one   = first;
    const finding_t *other = second;
    int              order = compare_numbers(one->line, other->line);
    order = order != 0 ? order : compare_numbers(one->column, other->column);
    return order != 0 ? order : compare_numbers(one->sequence, other->sequence);
}

void print_findings(const findings_t *findings, int file_count) {
    for (size_t index = 0; index < findings->count; index++) {
        const finding_t *finding = &findings->items[index];
        printf("%s:%lu:%lu ", finding->path, finding->line, finding->column);
        printf("%s %s\n", finding->rule, finding->message);
    }
    printf("%zu findings in %d files\n", findings->count, file_count);
}
