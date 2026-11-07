/** @file
 * Array and number helpers that the converter files share
 */

#pragma once

#include <stddef.h>

/** Number of items in an array. A pointer gives a wrong result */
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/** Bytes of a number text: the digits of UINT64_MAX, a point, decimals and a unit */
#define NUMBER_TEXT_SIZE 48

/** Text of a value that the logs cannot give, for example an average of 0 calls */
#define NO_VALUE_TEXT "-"

/** Milliseconds in one second */
#define MILLISECONDS_PER_SECOND 1000ULL

/** Decimals that change seconds to nanoseconds */
#define SECOND_TO_NANOSECOND_DECIMALS 9

/** Decimals of a time in microseconds, thus whole nanoseconds */
#define MICROSECOND_DECIMALS 3

/** Ratio of two numbers */
typedef struct {
    unsigned long long part;  /**< Dividend */
    unsigned long long whole; /**< Divisor, 0 for a ratio with no value */
} ratio_t;

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
 * Divides and rounds half up, with no floating point. A long division gives the
 * decimals, so the part is never multiplied and cannot overflow.
 *
 * @param ratio    Part and a whole that is not 0 and less than ULLONG_MAX / 10.
 * @param decimals Decimals of the quotient.
 * @return Quotient times 10 to the power of decimals.
 */
unsigned long long scaled_quotient(ratio_t ratio, size_t decimals);

/**
 * Writes a ratio as a decimal number with a fixed number of decimals, rounded half
 * up. Integer arithmetic gives the same text on all systems.
 *
 * @param text     Buffer of NUMBER_TEXT_SIZE bytes that gets the number, or
 *                 NO_VALUE_TEXT if the whole is 0.
 * @param ratio    Part and whole.
 * @param decimals Digits after the point, 1 or more.
 */
void format_ratio(char text[NUMBER_TEXT_SIZE], ratio_t ratio, size_t decimals);

/**
 * Writes a time in microseconds with MICROSECOND_DECIMALS decimals.
 *
 * @param text    Buffer of NUMBER_TEXT_SIZE bytes that gets microseconds, or
 *                NO_VALUE_TEXT if the whole is 0.
 * @param seconds Time in seconds as a ratio.
 */
void format_microseconds(char text[NUMBER_TEXT_SIZE], ratio_t seconds);
