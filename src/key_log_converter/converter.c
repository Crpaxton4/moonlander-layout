/** @file
 * Sessions, corpus text, heatmap lines, counts and profiling from the log records
 */

#include "converter.h"
#include "hid_usage.h"

/** Backspace pushes in this time after a push count against that position */
#define CORRECTION_WINDOW_MS 1000

/** Heatmap file line with no line break: keycode, row, column and layer */
#define HEATMAP_ROW_FORMAT "0x%04X,%u,%u,%u"

/**
 * Uses one record of one type.
 *
 * @param converter Outputs and totals.
 * @param record    Complete record.
 */
typedef void record_handler_t(converter_t *converter, const record_t *record);

/** Handler of one record type, and the logging state in which it applies */
typedef struct {
    record_handler_t *convert; /**< Use of the record */
    bool outside_sessions;     /**< true if the record also applies with logging off */
} record_rule_t;

/**
 * Starts a session. Keys from before the session are not new keys of it.
 *
 * @param converter Outputs and totals.
 * @param time      Time of the first record of the session.
 */
static void begin_session(converter_t *converter, uint32_t time) {
    converter->stream.in_session    = true;
    converter->stream.session_start = time;
    converter->stream.previous      = (report_t){.mods = 0};
    converter->recent_pushes.count  = 0;
    converter->summary.sessions++;
}

/**
 * Ends a session at the time of its last record. Each session ends with a line
 * break, so the next session starts on a new corpus line.
 *
 * @param converter Outputs and totals.
 */
static void end_session(converter_t *converter) {
    stream_t *stream   = &converter->stream;
    stream->in_session = false;
    converter->logged_ms += stream->time - stream->session_start;
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
        begin_session(converter, record->time);
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
 * Writes the heatmap line of a push and counts that line.
 *
 * @param converter Outputs and totals.
 * @param event     Key push.
 */
static void write_heatmap_row(converter_t *converter, const key_event_t *event) {
    char     line[COUNT_NAME_SIZE];
    unsigned keycode = event->keycode;
    unsigned row     = event->position.row;
    unsigned col     = event->position.col;
    unsigned layer   = event->layer;
    snprintf(line, sizeof(line), HEATMAP_ROW_FORMAT, keycode, row, col, layer);
    fprintf(converter->heatmap, "%s\n", line);
    add_named_count(&converter->heatmap_rows, line);
}

/**
 * Uses an input record. Only pushes go into the outputs, but each input record
 * makes its profiling interval a typing interval.
 *
 * @param converter Outputs and totals.
 * @param record    Input record.
 */
static void convert_input(converter_t *converter, const record_t *record) {
    const key_event_t *event    = &record->event;
    layer_position_t   key      = {.position = event->position, .layer = event->layer};
    converter->stream.has_input = true;
    converter->stream.last_input_time = record->time;
    if (!event->pushed) {
        return;
    }
    write_heatmap_row(converter, event);
    converter->key_pushes++;
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
 * Counts a name for a new key or for a key push with Ctrl, Alt or GUI.
 *
 * @param counts Table of counts.
 * @param press  New key and its modifiers.
 */
static void count_key_name(named_counts_t *counts, key_press_t press) {
    char name[SHORTCUT_NAME_SIZE];
    write_shortcut_name(press, name);
    add_named_count(counts, name);
}

/**
 * Uses a key that a report has and the report before it does not have.
 *
 * @param converter Outputs and totals.
 * @param press     New key and the modifiers of its report.
 */
static void convert_new_key(converter_t *converter, key_press_t press) {
    count_key_name(&converter->keys, (key_press_t){.usage = press.usage});
    if (press.usage == USAGE_BACKSPACE || press.usage == USAGE_DELETE) {
        converter->correction_pushes++;
    }
    if ((press.mods & MODS_SHORTCUT) != 0) {
        count_key_name(&converter->shortcuts, press);
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

/**
 * Uses a clock record, also with logging off. The firmware has one clock, so the
 * last clock record of the run applies to all handler records.
 *
 * @param converter Outputs and totals.
 * @param record    Clock record.
 */
static void convert_clock(converter_t *converter, const record_t *record) {
    converter->stream.has_clock = true;
    converter->profile.cpu_hz   = record->cpu_hz;
    if (converter->stream.in_session) {
        converter->profile.records++;
    }
}

/**
 * Uses a handler record. A record with no clock record before it in its file
 * comes from a cut log, so it also gets a count.
 *
 * @param converter Outputs and totals.
 * @param record    Handler record.
 */
static void convert_handler(converter_t *converter, const record_t *record) {
    profile_t *profile = &converter->profile;
    profile->records++;
    if (!converter->stream.has_clock) {
        profile->records_without_clock++;
    }
    add_handler_interval(profile, record, converter->summary.sessions);
}

/**
 * Finds an input record in the interval of a scan record. The interval is the
 * interval_ms before the record time, with the record time.
 *
 * @param stream State of the log file.
 * @param record Scan record.
 * @return true if the interval has an input record.
 */
static bool has_typing(const stream_t *stream, const record_t *record) {
    if (!stream->has_input) {
        return false;
    }
    return record->time - stream->last_input_time < record->scans.interval_ms;
}

/**
 * Uses a scan record.
 *
 * @param converter Outputs and totals.
 * @param record    Scan record.
 */
static void convert_scans(converter_t *converter, const record_t *record) {
    bool typing = has_typing(&converter->stream, record);
    converter->profile.records++;
    add_scan_interval(&converter->profile, &record->scans, typing);
}

/**
 * Uses a stack record.
 *
 * @param converter Outputs and totals.
 * @param record    Stack record.
 */
static void convert_stack(converter_t *converter, const record_t *record) {
    converter->profile.records++;
    add_stack_record(&converter->profile, record->stack_free_bytes);
}

/** Record rules, indexed by record_type_t */
static const record_rule_t record_rules[RECORD_TYPE_COUNT] = {
    [RECORD_STATE]   = {.convert = convert_state, .outside_sessions = true},
    [RECORD_INPUT]   = {.convert = convert_input},
    [RECORD_OUTPUT]  = {.convert = convert_output},
    [RECORD_CLOCK]   = {.convert = convert_clock, .outside_sessions = true},
    [RECORD_HANDLER] = {.convert = convert_handler},
    [RECORD_SCANS]   = {.convert = convert_scans},
    [RECORD_STACK]   = {.convert = convert_stack},
};

/**
 * Starts again after a restart or a timer wrap. Profiling is off after a
 * restart, so the file has no clock until its next clock record. An open session
 * ends, and a new one starts.
 *
 * @param converter Outputs and totals.
 * @param time      Time of the first record after the restart.
 */
static void restart_stream(converter_t *converter, uint32_t time) {
    stream_t *stream  = &converter->stream;
    stream->has_clock = false;
    stream->has_input = false;
    if (stream->in_session) {
        end_session(converter);
        begin_session(converter, time);
    }
}

void begin_stream(converter_t *converter) {
    converter->stream = (stream_t){.in_session = false};
    converter->summary.files++;
}

void convert_record(converter_t *converter, const record_t *record) {
    stream_t            *stream = &converter->stream;
    const record_rule_t *rule   = &record_rules[record->type];
    converter->summary.records++;
    // A time that goes back is a restart or a timer wrap
    if (stream->has_time && record->time < stream->time) {
        restart_stream(converter, record->time);
    }
    stream->has_time = true;
    stream->time     = record->time;
    if (!rule->outside_sessions && !stream->in_session) {
        converter->summary.records_outside_sessions++;
        return;
    }
    rule->convert(converter, record);
}

void end_stream(converter_t *converter) {
    if (converter->stream.in_session) {
        end_session(converter);
    }
}
