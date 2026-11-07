/** @file
 * Array helpers that the converter files share
 */

#pragma once

#include <stddef.h>

/** Number of items in an array. A pointer gives a wrong result */
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

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
