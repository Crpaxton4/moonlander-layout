/** @file
 * Sessions, corpus text, heatmap lines and counts from the log records
 */

#include "converter.h"
#include "hid_usage.h"

/** Backspace pushes in this time after a push count against that position */
#define CORRECTION_WINDOW_MS 1000

/** Line of the heatmap file: keycode, row, column and layer */
#define HEATMAP_FORMAT "0x%04X,%u,%u,%u\n"

/**
 * Uses one record of one type inside a session.
 *
 * @param converter Outputs and totals.
 * @param record    Complete record.
 */
typedef void record_handler_t(converter_t *converter, const record_t *record);

/**
 * Starts a session. Keys from before the session are not new keys of it.
 *
 * @param converter Outputs and totals.
 */
static void begin_session(converter_t *converter) {
    converter->stream.in_session   = true;
    converter->stream.previous     = (report_t){.mods = 0};
    converter->recent_pushes.count = 0;
    converter->summary.sessions++;
}

/**
 * Ends a session. Each session ends with a line break, so the next session starts
 * on a new corpus line.
 *
 * @param converter Outputs and totals.
 */
static void end_session(converter_t *converter) {
    converter->stream.in_session = false;
    if (converter->corpus_line_open) {
        fputc('\n', converter->corpus);
        converter->corpus_line_open = false;
    }
}

/**
 * Uses a state record. A second on in a session changes nothing.
 *
 * @param converter Outputs and totals.
 * @param record    State record.
 */
static void convert_state(converter_t *converter, const record_t *record) {
    bool switched_on = record->state == LOGGING_ON;
    bool in_session  = converter->stream.in_session;
    if (switched_on && !in_session) {
        begin_session(converter);
    } else if (!switched_on && in_session) {
        end_session(converter);
    }
}

/**
 * Counts a Backspace push against each position that had a push in the
 * correction window before it.
 *
 * @param converter Outputs and totals.
 * @param time      Time of the Backspace push.
 */
static void count_correction(converter_t *converter, uint32_t time) {
    for (size_t index = 0; index < converter->recent_pushes.count; index++) {
        const recent_push_t *push = &converter->recent_pushes.items[index];
        layer_position_t key = {.position = push->position, .layer = CORRECTION_LAYER};
        if (time - push->time < CORRECTION_WINDOW_MS) {
            find_position_count(&converter->corrections, key)->count++;
        }
    }
}

/**
 * Uses an input record. Only pushes go into the outputs.
 *
 * @param converter Outputs and totals.
 * @param record    Input record.
 */
static void convert_input(converter_t *converter, const record_t *record) {
    const key_event_t *event = &record->event;
    layer_position_t   key   = {.position = event->position, .layer = event->layer};
    if (!event->pushed) {
        return;
    }
    fprintf(converter->heatmap, HEATMAP_FORMAT, (unsigned)event->keycode,
            (unsigned)key.position.row, (unsigned)key.position.col,
            (unsigned)key.layer);
    find_position_count(&converter->usage, key)->count++;
    if (event->layer > converter->highest_layer) {
        converter->highest_layer = event->layer;
    }
    if (event->keycode == USAGE_BACKSPACE) {
        count_correction(converter, record->time);
    }
    set_recent_push(&converter->recent_pushes, event->position, record->time);
}

/**
 * Counts a key push with Ctrl, Alt or GUI.
 *
 * @param converter Outputs and totals.
 * @param press     New key and its modifiers.
 */
static void count_shortcut(converter_t *converter, key_press_t press) {
    char name[SHORTCUT_NAME_SIZE];
    write_shortcut_name(press, name);
    add_shortcut_count(&converter->shortcuts, name);
}

/**
 * Uses a key that a report has and the report before it does not have.
 *
 * @param converter Outputs and totals.
 * @param press     New key and the modifiers of its report.
 */
static void convert_new_key(converter_t *converter, key_press_t press) {
    if (press.usage == USAGE_BACKSPACE || press.usage == USAGE_DELETE) {
        converter->correction_pushes++;
    }
    if ((press.mods & MODS_SHORTCUT) != 0) {
        count_shortcut(converter, press);
        return;
    }
    char character = usage_character(press.usage, (press.mods & MODS_SHIFT) != 0);
    if (character == '\0') {
        return;
    }
    converter->character_pushes++;
    fputc(character, converter->corpus);
    converter->corpus_line_open = character != '\n';
}

/**
 * Finds a key in a report.
 *
 * @param report Keyboard report.
 * @param key    HID usage.
 * @return true if a slot of the report has the key.
 */
static bool report_has_key(const report_t *report, uint8_t key) {
    for (size_t slot = 0; slot < REPORT_KEY_COUNT; slot++) {
        if (report->keys[slot] == key) {
            return true;
        }
    }
    return false;
}

/**
 * Uses an output record. A key that stays down is new only in its first report.
 *
 * @param converter Outputs and totals.
 * @param record    Output record.
 */
static void convert_output(converter_t *converter, const record_t *record) {
    const report_t *report = &record->report;
    for (size_t slot = 0; slot < REPORT_KEY_COUNT; slot++) {
        key_press_t press = {.mods = report->mods, .usage = report->keys[slot]};
        if (press.usage != NO_KEY &&
            !report_has_key(&converter->stream.previous, press.usage)) {
            convert_new_key(converter, press);
        }
    }
    converter->stream.previous = *report;
}

/** Handlers, indexed by record_type_t */
static record_handler_t *const handlers[RECORD_TYPE_COUNT] = {
    [RECORD_STATE]  = convert_state,
    [RECORD_INPUT]  = convert_input,
    [RECORD_OUTPUT] = convert_output,
};

void begin_stream(converter_t *converter) {
    converter->stream = (stream_t){.in_session = false};
    converter->summary.files++;
}

void convert_record(converter_t *converter, const record_t *record) {
    stream_t *stream = &converter->stream;
    converter->summary.records++;
    // A time that goes back is a restart or a timer wrap, so a new session starts
    if (stream->has_time && record->time < stream->time && stream->in_session) {
        end_session(converter);
        begin_session(converter);
    }
    stream->has_time = true;
    stream->time     = record->time;
    if (record->type != RECORD_STATE && !stream->in_session) {
        converter->summary.records_outside_sessions++;
        return;
    }
    handlers[record->type](converter, record);
}

void end_stream(converter_t *converter) {
    if (converter->stream.in_session) {
        end_session(converter);
    }
}
