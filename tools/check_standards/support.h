/** @file
 * Support code shared by the checker's files: growing buffers and arrays, findings,
 * their order and their output. Each function is documented where it is defined, in
 * support.c. Doxygen takes the declaration of describe() for a function of its own,
 * as it carries a format attribute, so it is documented here too.
 */

#pragma once

#include <stddef.h>
#include <stdio.h>

/** Size of a finding's message, terminator included. */
#define MESSAGE_SIZE 160

/** Number of the first line and column in a finding; tree-sitter counts from 0. */
#define FIRST_POSITION 1

/** Characters that separate words on a line. */
#define BLANKS " \t"

/** Number of rows in the array @p array. */
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/** A growing block of bytes, NUL-terminated after each append(). */
typedef struct {
    /** The bytes. */
    char *data;
    /** Bytes in use, the terminator excluded. */
    size_t length;
    /** Bytes allocated. */
    size_t capacity;
} buffer_t;

/** One finding, as printed. */
typedef struct {
    /** Order in which the finding was reported, the last sort key. */
    size_t sequence;
    /** Line, counted from FIRST_POSITION. */
    unsigned long line;
    /** Column in bytes, counted from FIRST_POSITION. */
    unsigned long column;
    /** Path of the file as given. */
    const char *path;
    /** Name of the rule. */
    const char *rule;
    /** What is wrong. */
    char message[MESSAGE_SIZE];
} finding_t;

/** Every finding so far. */
typedef struct {
    /** The findings, in file order, each file's sorted once it is checked. */
    finding_t *items;
    /** Findings in use. */
    size_t count;
    /** Findings allocated. */
    size_t capacity;
} findings_t;

void *reserve(void *items, size_t item_size, size_t *capacity, size_t count);

void append(buffer_t *buffer, const char *text);

int append_stream(buffer_t *buffer, FILE *stream);

/**
 * Writes the message of a finding.
 *
 * @param finding The finding.
 * @param format  printf() format of the message, followed by its arguments.
 */
void describe(finding_t *finding, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

int compare_findings(const void *first, const void *second);

void print_findings(const findings_t *findings, int file_count);
