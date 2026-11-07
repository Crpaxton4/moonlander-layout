/** @file
 * Array growth for the converter tables
 */

#include <stdio.h>
#include <stdlib.h>

#include "support.h"

/** Capacity multiplier for a full array, so that appends stay fast on average */
#define GROWTH_FACTOR 2

/** Items added to each new capacity, so that small arrays do not grow often */
#define MIN_CAPACITY 16

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
