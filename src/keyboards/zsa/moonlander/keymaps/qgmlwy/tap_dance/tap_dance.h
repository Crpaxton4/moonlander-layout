/** @file
 * Tap dance slots of the qgmlwy keymap, the shape of their definitions and the
 * QMK callbacks that play them.
 *
 * Every dance is one row of `dances`: a keycode dance names the key each outcome
 * holds, a layer dance the layer each outcome moves to. One set of callbacks plays
 * every row, and the keymap's `tap_dance_actions` binds each slot to its row with
 * ACTION_TAP_DANCE_TABLE().
 */

#pragma once

#include QMK_KEYBOARD_H

/**
 * Tap dance slots, each the index of its action in `tap_dance_actions` and of its
 * definition in `dances`.
 */
enum tap_dance_codes {
    DANCE_0,
    DANCE_1,
    DANCE_2,
    DANCE_3,
    DANCE_4,
    DANCE_5,
    DANCE_6,
    DANCE_7,
    DANCE_8,
    DANCE_9,
    DANCE_10,
    DANCE_11,
    DANCE_12,
    DANCE_13,
    DANCE_14,
    DANCE_15,
    DANCE_16,
    /** Number of tap dance slots; sizes `dances`. */
    DANCE_COUNT,
};

/** What a dance's outcomes do. */
typedef enum {
    /** Each outcome holds a key until the dance resets. */
    DANCE_KEYCODES,
    /** Each outcome moves to a layer, which stays after the dance resets. */
    DANCE_LAYERS,
} dance_kind_t;

/**
 * Keys of a keycode dance, one per outcome; KC_NO makes an outcome do nothing.
 *
 * Two presses interrupted by another key tap the tap key once, then hold it.
 */
typedef struct {
    /** Held on a single tap; tapped, then held, on an interrupted double tap. */
    uint16_t tap;
    /** Held while the dance key is held. */
    uint16_t hold;
    /** Held on a double tap. */
    uint16_t double_tap;
    /** Held on a tap followed by a hold. */
    uint16_t double_hold;
    /** Registers the double-tap key twice on a double tap; one release follows. */
    bool registers_double_tap_twice;
    /** Taps the tap key three times on a third tap, then once per further tap. */
    bool repeats_on_triple_tap;
} keycode_dance_t;

/**
 * Layers of a layer dance, one per outcome; NO_LAYER makes an outcome do nothing.
 *
 * Two presses interrupted by another key move to the tap layer.
 */
typedef struct {
    /** Moved to on a single tap or an interrupted double tap. */
    uint8_t tap;
    /** Moved to while the dance key is held. */
    uint8_t hold;
    /** Moved to on a double tap. */
    uint8_t double_tap;
    /** Moved to on a tap followed by a hold. */
    uint8_t double_hold;
} layer_dance_t;

/** Definition of one tap dance: its kind and what each outcome sends. */
typedef struct {
    /** Which member of the union holds the definition. */
    dance_kind_t kind;
    union {
        /** Keys of a DANCE_KEYCODES dance. */
        keycode_dance_t keycodes;
        /** Layers of a DANCE_LAYERS dance. */
        layer_dance_t layers;
    };
} dance_t;

/** Layer of a layer dance outcome that does nothing; layer 0 is a real layer. */
#define NO_LAYER UINT8_MAX

/**
 * Keycode dance definition from a braced list of designated keycode_dance_t
 * fields.
 *
 * Fields left out are KC_NO or false.
 */
#define KEYCODE_DANCE(...) {.kind = DANCE_KEYCODES, .keycodes = __VA_ARGS__}

/**
 * Layer dance definition.
 *
 * Every outcome is named, NO_LAYER for none, because a left-out field would be
 * layer 0.
 */
#define LAYER_DANCE(tap_layer, hold_layer, double_tap_layer, double_hold_layer) \
    {                                                                           \
        .kind = DANCE_LAYERS, .layers = {                                       \
            .tap         = (tap_layer),                                         \
            .hold        = (hold_layer),                                        \
            .double_tap  = (double_tap_layer),                                  \
            .double_hold = (double_hold_layer),                                 \
        }                                                                       \
    }

/** Definitions of the tap dances, indexed by their slot. */
extern const dance_t dances[DANCE_COUNT];

/**
 * QMK tap dance action that plays the definition in `dances` at a slot.
 *
 * QMK hands the definition back to each callback as its `user_data`, which QMK
 * types as mutable; the callbacks only read it.
 */
#define ACTION_TAP_DANCE_TABLE(slot)                                         \
    {                                                                        \
        .fn        = {dance_on_each_tap, dance_finished, dance_reset, NULL}, \
        .user_data = (void *)&dances[(slot)],                                \
    }

/**
 * Repeats a keycode dance's tap key on long tap runs, if its definition asks for
 * that: three times at the third tap, then once per further tap.
 *
 * @param state     Tap count of the dance.
 * @param user_data The dance's definition in `dances`.
 */
void dance_on_each_tap(tap_dance_state_t *state, void *user_data);

/**
 * Resolves a dance's taps to one outcome, records it, and holds that outcome's key
 * or moves to its layer.
 *
 * @param state     Tap count and key state of the dance.
 * @param user_data The dance's definition in `dances`.
 */
void dance_finished(tap_dance_state_t *state, void *user_data);

/**
 * Waits 10 ms, releases the key dance_finished() held, if any, and clears the
 * dance's outcome.
 *
 * @param state     Tap count and key state of the dance; unused.
 * @param user_data The dance's definition in `dances`.
 */
void dance_reset(tap_dance_state_t *state, void *user_data);
