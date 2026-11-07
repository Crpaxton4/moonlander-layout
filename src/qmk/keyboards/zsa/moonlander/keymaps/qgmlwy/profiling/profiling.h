/** @file
 * Profiling debug build of the qgmlwy keymap: core cycles of the key event hook and
 * of each record handler, matrix scans and free stack, written on the QMK console
 * while profiling is on.
 *
 * Each record is one line that starts with `KL,`, as the key event log records. A
 * clock record gives the core clock, a profile record the calls and cycles of one
 * measured call, a rate record the matrix scans, and a stack record the free stack.
 *
 * rules.mk compiles the profiling sources only for `DEBUG_PROFILE = yes`. In the
 * normal firmware, the two measure macros are plain calls, so its bytes do not
 * change.
 */

#pragma once

#include QMK_KEYBOARD_H

#include "profiling/profiling_logic.h"

/** Key event handler that profiling measures, as in the record handler table */
typedef bool (*measured_handler_t)(uint16_t keycode, keyrecord_t *record);

/**
 * Switches profiling on or off for the toggle key. Gives each other key event to
 * the dispatcher and measures that call while profiling is on.
 *
 * @param dispatcher Function that gives the event to the record handlers.
 * @param keycode    Keycode of the key event.
 * @param record     Key event.
 * @return False for the toggle key, which sends no key; else the dispatcher result.
 */
bool process_profiled_event(measured_handler_t dispatcher, uint16_t keycode,
                            keyrecord_t *record);

/**
 * Gives a key event to a record handler, and measures that call while profiling is
 * on.
 *
 * @param index   Index of the handler in the record handler table.
 * @param handler Record handler.
 * @param keycode Keycode of the key event.
 * @param record  Key event.
 * @return Result of the handler.
 */
bool profile_record_handler(size_t index, measured_handler_t handler, uint16_t keycode,
                            keyrecord_t *record);

/**
 * Tells if profiling is on.
 *
 * @return True while profiling is on.
 */
bool profiling_is_on(void);

#ifdef DEBUG_PROFILE
/** Measured call of the dispatcher of process_record_user() */
#    define PROFILED_RECORD_USER(dispatcher, keycode, record) \
        process_profiled_event(dispatcher, keycode, record)

/** Measured call of the record handler at an index of the record handler table */
#    define PROFILED_RECORD_HANDLER(index, handler, keycode, record) \
        profile_record_handler(index, handler, keycode, record)
#else
/** Plain call of the dispatcher of process_record_user() */
#    define PROFILED_RECORD_USER(dispatcher, keycode, record) \
        dispatcher(keycode, record)

/** Plain call of the record handler at an index of the record handler table */
#    define PROFILED_RECORD_HANDLER(index, handler, keycode, record) \
        handler(keycode, record)
#endif
