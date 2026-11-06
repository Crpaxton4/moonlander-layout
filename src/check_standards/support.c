/** @file
 * The buffers, the findings and the output of the checker.
 */

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support.h"

#define READ_CHUNK_SIZE 4096

#define GROWTH_FACTOR 2

#define MIN_CAPACITY 16

/** The array can move, so use the return value. Without memory, the program stops. */
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

/** Makes space for extra bytes and also for the terminator. */
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

/** Returns 0, or the errno value of a read error. It adds no terminator. */
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

/** A subtraction of size_t values can wrap, but this form cannot. */
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
