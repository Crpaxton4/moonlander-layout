/** @file
 * Count tables of the converter: pushes per position, recent pushes and shortcuts
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#include "hid_usage.h"
#include "record.h"

/** Rows of the Moonlander (rev A) QMK matrix, both halves */
#define MATRIX_ROWS 12

/** Columns of the Moonlander (rev A) QMK matrix */
#define MATRIX_COLS 7

/** Layer of each correction count, because that table has no layers */
#define CORRECTION_LAYER 0

/** Number of pushes or corrections */
typedef unsigned long long count_t;

/** Key position on one layer */
typedef struct {
    position_t position; /**< Key position */
    uint8_t    layer;    /**< Layer, CORRECTION_LAYER in a table without layers */
} layer_position_t;

/** Count of one position on one layer */
typedef struct {
    layer_position_t key;   /**< Position and layer */
    count_t          count; /**< Pushes or corrections */
} position_count_t;

/** Counts of positions, in no order until sort_position_counts() */
typedef struct {
    position_count_t *items;    /**< Counts, or NULL before the first count */
    size_t            count;    /**< Items in use */
    size_t            capacity; /**< Items that the array can hold */
} position_counts_t;

/** Last push time of one position */
typedef struct {
    position_t position; /**< Key position */
    uint32_t   time;     /**< Time of the last push */
} recent_push_t;

/** Last push time of each position in the current session */
typedef struct {
    recent_push_t *items;    /**< Pushes, or NULL before the first push */
    size_t         count;    /**< Items in use */
    size_t         capacity; /**< Items that the array can hold */
} recent_pushes_t;

/** Count of one shortcut */
typedef struct {
    char    name[SHORTCUT_NAME_SIZE]; /**< Name from write_shortcut_name() */
    count_t count;                    /**< Pushes */
} shortcut_count_t;

/** Counts of shortcuts, in no order until sort_shortcut_counts() */
typedef struct {
    shortcut_count_t *items;    /**< Counts, or NULL before the first count */
    size_t            count;    /**< Items in use */
    size_t            capacity; /**< Items that the array can hold */
} shortcut_counts_t;

/**
 * Finds the count of a position and layer, and adds a zero count if none exists.
 *
 * @param counts Table of counts.
 * @param key    Position and layer.
 * @return Count in the table. The next add can move it.
 */
position_count_t *find_position_count(position_counts_t *counts, layer_position_t key);

/**
 * Adds a zero count for each matrix position of a layer that has no count.
 *
 * @param counts Table of counts.
 * @param layer  Layer.
 */
void add_matrix_positions(position_counts_t *counts, uint8_t layer);

/**
 * Sorts counts by layer, row and column.
 *
 * @param counts Table of counts.
 */
void sort_position_counts(position_counts_t *counts);

/**
 * Sets the last push time of a position.
 *
 * @param pushes   Recent pushes.
 * @param position Key position.
 * @param time     Time of the push.
 */
void set_recent_push(recent_pushes_t *pushes, position_t position, uint32_t time);

/**
 * Adds one to the shortcut count of a name.
 *
 * @param counts Table of counts.
 * @param name   Shortcut name.
 */
void add_shortcut_count(shortcut_counts_t *counts, const char *name);

/**
 * Sorts shortcuts from the largest count. Equal counts are in name order.
 *
 * @param counts Table of counts.
 */
void sort_shortcut_counts(shortcut_counts_t *counts);
