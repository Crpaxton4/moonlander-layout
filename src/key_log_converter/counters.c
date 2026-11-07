/** @file
 * Count tables with a linear search. A log has a few hundred positions at most
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "counters.h"
#include "support.h"

/**
 * Compares two positions.
 *
 * @param left  First position.
 * @param right Second position.
 * @return true if row and column are equal.
 */
static bool same_position(position_t left, position_t right) {
    if (left.row != right.row) {
        return false;
    }
    return left.col == right.col;
}

/**
 * Compares two positions on layers.
 *
 * @param left  First position.
 * @param right Second position.
 * @return true if layer, row and column are equal.
 */
static bool same_key(layer_position_t left, layer_position_t right) {
    if (left.layer != right.layer) {
        return false;
    }
    return same_position(left.position, right.position);
}

position_count_t *find_position_count(position_counts_t *counts, layer_position_t key) {
    for (size_t index = 0; index < counts->count; index++) {
        if (same_key(counts->items[index].key, key)) {
            return &counts->items[index];
        }
    }
    counts->items = reserve(counts->items, sizeof(position_count_t), &counts->capacity,
                            counts->count);
    position_count_t *item = &counts->items[counts->count++];
    *item                  = (position_count_t){.key = key, .count = 0};
    return item;
}

void add_matrix_positions(position_counts_t *counts, uint8_t layer) {
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            layer_position_t key = {.position = {.row = row, .col = col},
                                    .layer    = layer};
            find_position_count(counts, key);
        }
    }
}

/**
 * Compares two numbers. A subtraction of unsigned values can wrap, but this form
 * cannot.
 *
 * @param left  First number.
 * @param right Second number.
 * @return -1, 0 or 1.
 */
static int compare_numbers(count_t left, count_t right) {
    return (left > right) - (left < right);
}

/**
 * Orders position counts by layer, row and column, for qsort().
 *
 * @param first  First position_count_t.
 * @param second Second position_count_t.
 * @return Negative, 0 or positive, as qsort() requires.
 */
static int compare_position_counts(const void *first, const void *second) {
    layer_position_t one   = ((const position_count_t *)first)->key;
    layer_position_t other = ((const position_count_t *)second)->key;
    int              order = compare_numbers(one.layer, other.layer);
    if (order == 0) {
        order = compare_numbers(one.position.row, other.position.row);
    }
    if (order == 0) {
        order = compare_numbers(one.position.col, other.position.col);
    }
    return order;
}

void sort_position_counts(position_counts_t *counts) {
    size_t item_size = sizeof(position_count_t);
    if (counts->count > 0) {
        qsort(counts->items, counts->count, item_size, compare_position_counts);
    }
}

void set_recent_push(recent_pushes_t *pushes, position_t position, uint32_t time) {
    for (size_t index = 0; index < pushes->count; index++) {
        if (same_position(pushes->items[index].position, position)) {
            pushes->items[index].time = time;
            return;
        }
    }
    pushes->items =
        reserve(pushes->items, sizeof(recent_push_t), &pushes->capacity, pushes->count);
    pushes->items[pushes->count++] =
        (recent_push_t){.position = position, .time = time};
}

/**
 * Finds the count of a shortcut, and adds a zero count if none exists.
 *
 * @param counts Table of counts.
 * @param name   Shortcut name.
 * @return Count in the table. The next add can move it.
 */
static shortcut_count_t *find_shortcut(shortcut_counts_t *counts, const char *name) {
    for (size_t index = 0; index < counts->count; index++) {
        if (strcmp(counts->items[index].name, name) == 0) {
            return &counts->items[index];
        }
    }
    counts->items = reserve(counts->items, sizeof(shortcut_count_t), &counts->capacity,
                            counts->count);
    shortcut_count_t *item = &counts->items[counts->count++];
    snprintf(item->name, sizeof(item->name), "%s", name);
    item->count = 0;
    return item;
}

void add_shortcut_count(shortcut_counts_t *counts, const char *name) {
    find_shortcut(counts, name)->count++;
}

/**
 * Orders shortcut counts from the largest count, then by name, for qsort().
 *
 * @param first  First shortcut_count_t.
 * @param second Second shortcut_count_t.
 * @return Negative, 0 or positive, as qsort() requires.
 */
static int compare_shortcut_counts(const void *first, const void *second) {
    const shortcut_count_t *one   = first;
    const shortcut_count_t *other = second;
    int                     order = compare_numbers(other->count, one->count);
    return order != 0 ? order : strcmp(one->name, other->name);
}

void sort_shortcut_counts(shortcut_counts_t *counts) {
    size_t item_size = sizeof(shortcut_count_t);
    if (counts->count > 0) {
        qsort(counts->items, counts->count, item_size, compare_shortcut_counts);
    }
}
