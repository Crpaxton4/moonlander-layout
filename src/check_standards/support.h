/** @file
 * The interface of support.c and the types that the checker files share.
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

#define MESSAGE_SIZE 160

/* tree-sitter counts lines and columns from 0, but a finding counts from 1. */
#define FIRST_POSITION 1

#define BLANKS " \t"

#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/** append() keeps the bytes NUL-terminated, but append_stream() does not. */
typedef struct {
    char  *data;
    size_t length;
    size_t capacity;
} buffer_t;

typedef struct {
    /** The report order. It is the last sort key, so the sort is stable. */
    size_t        sequence;
    unsigned long line;
    /** The column counts bytes, not characters. */
    unsigned long column;
    const char   *path;
    const char   *rule;
    char          message[MESSAGE_SIZE];
} finding_t;

typedef struct {
    /** The findings of each file are in sort order after the check of that file. */
    finding_t *items;
    size_t     count;
    size_t     capacity;
} findings_t;

void *reserve(void *items, size_t item_size, size_t *capacity, size_t count);

void append(buffer_t *buffer, const char *text);

int append_stream(buffer_t *buffer, FILE *stream);

void describe(finding_t *finding, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

int compare_findings(const void *first, const void *second);

void print_findings(const findings_t *findings, int file_count);
