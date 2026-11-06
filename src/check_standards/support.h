/** @file
 * The code that the checker files share: buffers and arrays that grow, and the
 * findings with their sort order and output. The documentation of each function is
 * at its definition in support.c. Doxygen reads the declaration of describe() as a
 * different function because of its format attribute. Thus describe() also has its
 * documentation here.
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

/** The size of a finding message. The size includes the terminator. */
#define MESSAGE_SIZE 160

/** The number of the first line and column in a finding. tree-sitter counts from 0. */
#define FIRST_POSITION 1

/** The characters that separate words on a line. */
#define BLANKS " \t"

/** The number of elements in the array @p array. */
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/** A block of bytes that grows. append() puts a NUL after the bytes. */
typedef struct {
    /** The bytes. */
    char *data;
    /** The number of bytes in use, without the terminator. */
    size_t length;
    /** The number of bytes that are allocated. */
    size_t capacity;
} buffer_t;

/** One finding, as the checker prints it. */
typedef struct {
    /** The report order of the finding. It is the last sort key. */
    size_t sequence;
    /** The line. The first line is FIRST_POSITION. */
    unsigned long line;
    /** The column in bytes. The first column is FIRST_POSITION. */
    unsigned long column;
    /** The path of the file, as the command line gives it. */
    const char *path;
    /** The name of the rule. */
    const char *rule;
    /** The description of the problem. */
    char message[MESSAGE_SIZE];
} finding_t;

/** All findings until now. */
typedef struct {
    /** The findings in file order. Each file has its findings in sort order. */
    finding_t *items;
    /** The number of findings in use. */
    size_t count;
    /** The number of findings that are allocated. */
    size_t capacity;
} findings_t;

void *reserve(void *items, size_t item_size, size_t *capacity, size_t count);

void append(buffer_t *buffer, const char *text);

int append_stream(buffer_t *buffer, FILE *stream);

/**
 * Writes the message of a finding.
 *
 * @param finding The finding.
 * @param format  The printf() format of the message. The format arguments follow it.
 */
void describe(finding_t *finding, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

int compare_findings(const void *first, const void *second);

void print_findings(const findings_t *findings, int file_count);
