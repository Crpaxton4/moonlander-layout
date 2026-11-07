/** @file
 * Array growth for the converter tables, and fixed-point number texts
 */

#include <stdio.h>
#include <stdlib.h>

#include "support.h"

/** Capacity multiplier for a full array, so that appends stay fast on average */
#define GROWTH_FACTOR 2

/** Items added to each new capacity, so that small arrays do not grow often */
#define MIN_CAPACITY 16

/** Base of the decimal digits */
#define DECIMAL_BASE 10ULL

/** Scale of a number with no decimals */
#define NO_DECIMALS_SCALE 1ULL

/** Decimal number as an integer and its number of decimals */
typedef struct {
    unsigned long long scaled;   /**< Number times 10 to the power of decimals */
    size_t             decimals; /**< Digits after the point, 1 or more */
} fixed_point_t;

void *reserve(void *items, size_t item_size, size_t *capacity, size_t count) {
    if (count < *capacity) {
        return items;
    }
    *capacity   = count * GROWTH_FACTOR + MIN_CAPACITY;
    void *grown = realloc(items, *capacity * item_size);
    if (grown == NULL) {
        perror("key_log_converter");
        abort();
    }
    return grown;
}

unsigned long long scaled_quotient(ratio_t ratio, size_t decimals) {
    unsigned long long quotient  = ratio.part / ratio.whole;
    unsigned long long remainder = ratio.part % ratio.whole;
    for (size_t digit = 0; digit < decimals; digit++) {
        remainder *= DECIMAL_BASE;
        quotient = quotient * DECIMAL_BASE + remainder / ratio.whole;
        remainder %= ratio.whole;
    }
    // Half up: a remainder of at least half the whole adds one
    if (remainder >= ratio.whole - remainder) {
        quotient++;
    }
    return quotient;
}

/**
 * Writes a number with a fixed number of decimals.
 *
 * @param text   Buffer of NUMBER_TEXT_SIZE bytes that gets the number.
 * @param number Number and its decimals.
 */
static void format_fixed(char *text, fixed_point_t number) {
    unsigned long long scale = NO_DECIMALS_SCALE;
    for (size_t digit = 0; digit < number.decimals; digit++) {
        scale *= DECIMAL_BASE;
    }
    unsigned long long whole_part = number.scaled / scale;
    unsigned long long fraction   = number.scaled % scale;
    int                width      = (int)number.decimals;
    snprintf(text, NUMBER_TEXT_SIZE, "%llu.%0*llu", whole_part, width, fraction);
}

void format_ratio(char text[NUMBER_TEXT_SIZE], ratio_t ratio, size_t decimals) {
    if (ratio.whole == 0) {
        snprintf(text, NUMBER_TEXT_SIZE, "%s", NO_VALUE_TEXT);
        return;
    }
    fixed_point_t number = {
        .scaled   = scaled_quotient(ratio, decimals),
        .decimals = decimals,
    };
    format_fixed(text, number);
}

void format_microseconds(char text[NUMBER_TEXT_SIZE], ratio_t seconds) {
    if (seconds.whole == 0) {
        snprintf(text, NUMBER_TEXT_SIZE, "%s", NO_VALUE_TEXT);
        return;
    }
    // Whole nanoseconds are microseconds with MICROSECOND_DECIMALS decimals
    fixed_point_t microseconds = {
        .scaled   = scaled_quotient(seconds, SECOND_TO_NANOSECOND_DECIMALS),
        .decimals = MICROSECOND_DECIMALS,
    };
    format_fixed(text, microseconds);
}
