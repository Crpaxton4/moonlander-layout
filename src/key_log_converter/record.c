/** @file
 * Parser of the log records. Tables give the fields of each record type
 */

// #region Includes

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "record.h"
#include "support.h"

// #endregion

// #region Macros

/** Character between two fields */
#define FIELD_SEPARATOR ','

/** First field of each record */
#define RECORD_MARKER "KL"

/** Characters that hid_listen writes after a status text while it waits */
#define STATUS_PROGRESS "."

/** Most fields of a record type */
#define MAX_FIELD_COUNT INPUT_FIELD_COUNT

/** Base of a decimal number field */
#define DECIMAL_BASE 10

/** Base of a hexadecimal number field */
#define HEX_BASE 16

/** Characters of a handler name. Other characters can break the Markdown report */
#define NAME_CHARACTERS \
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"

/** Field count that split_fields() gives for a line with too many fields */
#define TOO_MANY_FIELDS (MAX_FIELD_COUNT + 1)

// #endregion

// #region Enumerations

/** Field positions that all record types share */
enum {
    FIELD_MARKER, /**< RECORD_MARKER */
    FIELD_TYPE,   /**< Record type letter */
    FIELD_TIME,   /**< Milliseconds since power-up */
};

/** Field positions of a state record */
enum {
    STATE_FIELD_STATE = FIELD_TIME + 1, /**< on, off or idle-off */
    STATE_FIELD_COUNT,                  /**< Fields of a state record */
};

/** Field positions of an input record */
enum {
    INPUT_FIELD_ACTION = FIELD_TIME + 1, /**< d for a push, u for a release */
    INPUT_FIELD_ROW,                     /**< Matrix row */
    INPUT_FIELD_COL,                     /**< Matrix column */
    INPUT_FIELD_LAYER,                   /**< Highest active layer */
    INPUT_FIELD_KEYCODE,                 /**< Keycode after layer resolution */
    INPUT_FIELD_MODS,                    /**< Active modifiers */
    INPUT_FIELD_ONE_SHOT_MODS,           /**< Active one-shot modifiers */
    INPUT_FIELD_TAP_COUNT,               /**< Tap count of a tap-hold key */
    INPUT_FIELD_INTERRUPTED,             /**< 1 if another key interrupted the tap */
    INPUT_FIELD_COUNT,                   /**< Fields of an input record */
};

/** Field positions of an output record */
enum {
    OUTPUT_FIELD_MODS = FIELD_TIME + 1, /**< HID modifier byte */
    OUTPUT_FIELD_FIRST_KEY,             /**< First of the REPORT_KEY_COUNT key codes */
    OUTPUT_FIELD_COUNT = OUTPUT_FIELD_FIRST_KEY + REPORT_KEY_COUNT, /**< Fields */
};

/** Field positions of a clock record */
enum {
    CLOCK_FIELD_CPU_HZ = FIELD_TIME + 1, /**< Core clock in Hz */
    CLOCK_FIELD_COUNT,                   /**< Fields of a clock record */
};

/** Field positions of a handler record */
enum {
    HANDLER_FIELD_NAME = FIELD_TIME + 1, /**< Handler name */
    HANDLER_FIELD_CALLS,                 /**< Calls in the interval */
    HANDLER_FIELD_CYCLE_SUM,             /**< CPU cycles of all calls */
    HANDLER_FIELD_CYCLE_MAX,             /**< CPU cycles of the slowest call */
    HANDLER_FIELD_COUNT,                 /**< Fields of a handler record */
};

/** Field positions of a scan record */
enum {
    SCANS_FIELD_SCAN_COUNT = FIELD_TIME + 1, /**< Matrix scans in the interval */
    SCANS_FIELD_INTERVAL_MS,                 /**< Interval length in milliseconds */
    SCANS_FIELD_COUNT,                       /**< Fields of a scan record */
};

/** Field positions of a stack record */
enum {
    STACK_FIELD_FREE_BYTES = FIELD_TIME + 1, /**< Lowest free main stack */
    STACK_FIELD_COUNT,                       /**< Fields of a stack record */
};

/**
 * Value formats of the fields. FORMAT_HEX_BYTE is the zero value, so a field that a
 * format table does not list is a hexadecimal byte.
 */
typedef enum {
    FORMAT_HEX_BYTE,     /**< Hexadecimal number up to FF */
    FORMAT_TIME,         /**< Decimal 32-bit number */
    FORMAT_STATE,        /**< Word of state_words */
    FORMAT_ACTION,       /**< Word of action_words */
    FORMAT_DECIMAL_BYTE, /**< Decimal number up to 255 */
    FORMAT_HEX_KEYCODE,  /**< Hexadecimal number up to FFFF */
    FORMAT_DECIMAL_FLAG, /**< 0 or 1 */
    FORMAT_DECIMAL,      /**< Decimal 32-bit number */
    FORMAT_POSITIVE,     /**< Decimal 32-bit number that is not 0, thus a divisor */
    FORMAT_NAME,         /**< Handler name of NAME_CHARACTERS */
    FORMAT_COUNT,        /**< Number of formats */
} field_format_t;

// #endregion

// #region Types

/** Value of a field */
typedef unsigned long long field_value_t;

/** Text and value of one field */
typedef struct {
    const char   *text;  /**< Field text, terminated at its separator */
    field_value_t value; /**< Value after a parse */
} field_t;

/** Word of a field and the value that it gives */
typedef struct {
    const char   *text;  /**< Word in the log */
    field_value_t value; /**< Value of the word */
} word_t;

/** Rules of one field format, declared before field_parser_t because it uses them */
typedef struct format_rule format_rule_t;

/**
 * Parses one field.
 *
 * @param rule  Rules of the field format.
 * @param field Field that gets its value.
 * @return true for a valid field.
 */
typedef bool field_parser_t(const format_rule_t *rule, field_t *field);

/** Rules of one field format: a parser and its words, number range or name length */
struct format_rule {
    field_parser_t *parse;   /**< Parser of the field */
    const word_t   *words;   /**< Words of parse_word(), up to a NULL text */
    field_value_t   minimum; /**< Smallest permitted number */
    field_value_t   maximum; /**< Largest permitted number or name length */
};

/**
 * Copies the field values of one record type into a record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
typedef void record_store_t(const field_t *fields, record_t *record);

/** Field layout of one record type */
typedef struct {
    const char           *letter;      /**< Second field of the record */
    record_type_t         type;        /**< Record type */
    size_t                field_count; /**< Fields of the record */
    const field_format_t *formats;     /**< Format of each field */
    record_store_t       *store;       /**< Copy of the values into a record */
} record_layout_t;

// #endregion

// #region Function declarations

static bool parse_decimal(const format_rule_t *rule, field_t *field);
static bool parse_hex(const format_rule_t *rule, field_t *field);
static bool parse_name(const format_rule_t *rule, field_t *field);
static bool parse_word(const format_rule_t *rule, field_t *field);
static void store_state(const field_t *fields, record_t *record);
static void store_input(const field_t *fields, record_t *record);
static void store_output(const field_t *fields, record_t *record);
static void store_clock(const field_t *fields, record_t *record);
static void store_handler(const field_t *fields, record_t *record);
static void store_scans(const field_t *fields, record_t *record);
static void store_stack(const field_t *fields, record_t *record);

// #endregion

// #region Data

/**
 * Status texts of hid_listen. The text can be in front of a record on the same
 * line, so the parser removes it
 */
static const char *const status_texts[] = {
    "Waiting for device:",
    "Waiting for new device:",
    "Listening:",
    "Device disconnected.",
};

/** Words of a state record, with logging_state_t values */
static const word_t state_words[] = {
    {"on", LOGGING_ON},
    {"off", LOGGING_OFF},
    {"idle-off", LOGGING_IDLE_OFF},
    {NULL, 0},
};

/** Words of an input record action */
static const word_t action_words[] = {
    {"d", true},
    {"u", false},
    {NULL, 0},
};

/** Format rules, indexed by field_format_t */
static const format_rule_t format_rules[FORMAT_COUNT] = {
    [FORMAT_TIME]         = {.parse = parse_decimal, .maximum = UINT32_MAX},
    [FORMAT_STATE]        = {.parse = parse_word, .words = state_words},
    [FORMAT_ACTION]       = {.parse = parse_word, .words = action_words},
    [FORMAT_DECIMAL_BYTE] = {.parse = parse_decimal, .maximum = UINT8_MAX},
    [FORMAT_HEX_BYTE]     = {.parse = parse_hex, .maximum = UINT8_MAX},
    [FORMAT_HEX_KEYCODE]  = {.parse = parse_hex, .maximum = UINT16_MAX},
    [FORMAT_DECIMAL_FLAG] = {.parse = parse_decimal, .maximum = true},
    // Formats of the profiling records
    [FORMAT_DECIMAL]  = {.parse = parse_decimal, .maximum = UINT32_MAX},
    [FORMAT_POSITIVE] = {.parse = parse_decimal, .minimum = 1, .maximum = UINT32_MAX},
    [FORMAT_NAME]     = {.parse = parse_name, .maximum = HANDLER_NAME_SIZE - 1},
};

/** Formats of the state record fields */
static const field_format_t state_formats[STATE_FIELD_COUNT] = {
    [FIELD_TIME]        = FORMAT_TIME,
    [STATE_FIELD_STATE] = FORMAT_STATE,
};

/** Formats of the input record fields */
static const field_format_t input_formats[INPUT_FIELD_COUNT] = {
    [FIELD_TIME]                = FORMAT_TIME,
    [INPUT_FIELD_ACTION]        = FORMAT_ACTION,
    [INPUT_FIELD_ROW]           = FORMAT_DECIMAL_BYTE,
    [INPUT_FIELD_COL]           = FORMAT_DECIMAL_BYTE,
    [INPUT_FIELD_LAYER]         = FORMAT_DECIMAL_BYTE,
    [INPUT_FIELD_KEYCODE]       = FORMAT_HEX_KEYCODE,
    [INPUT_FIELD_MODS]          = FORMAT_HEX_BYTE,
    [INPUT_FIELD_ONE_SHOT_MODS] = FORMAT_HEX_BYTE,
    [INPUT_FIELD_TAP_COUNT]     = FORMAT_DECIMAL_BYTE,
    [INPUT_FIELD_INTERRUPTED]   = FORMAT_DECIMAL_FLAG,
};

/** Formats of the output record fields. The key codes are not listed, thus hex bytes */
static const field_format_t output_formats[OUTPUT_FIELD_COUNT] = {
    [FIELD_TIME]        = FORMAT_TIME,
    [OUTPUT_FIELD_MODS] = FORMAT_HEX_BYTE,
};

/** Formats of the clock record fields */
static const field_format_t clock_formats[CLOCK_FIELD_COUNT] = {
    [FIELD_TIME]         = FORMAT_TIME,
    [CLOCK_FIELD_CPU_HZ] = FORMAT_POSITIVE,
};

/** Formats of the handler record fields */
static const field_format_t handler_formats[HANDLER_FIELD_COUNT] = {
    [FIELD_TIME]              = FORMAT_TIME,
    [HANDLER_FIELD_NAME]      = FORMAT_NAME,
    [HANDLER_FIELD_CALLS]     = FORMAT_DECIMAL,
    [HANDLER_FIELD_CYCLE_SUM] = FORMAT_DECIMAL,
    [HANDLER_FIELD_CYCLE_MAX] = FORMAT_DECIMAL,
};

/** Formats of the scan record fields */
static const field_format_t scans_formats[SCANS_FIELD_COUNT] = {
    [FIELD_TIME]              = FORMAT_TIME,
    [SCANS_FIELD_SCAN_COUNT]  = FORMAT_DECIMAL,
    [SCANS_FIELD_INTERVAL_MS] = FORMAT_POSITIVE,
};

/** Formats of the stack record fields */
static const field_format_t stack_formats[STACK_FIELD_COUNT] = {
    [FIELD_TIME]             = FORMAT_TIME,
    [STACK_FIELD_FREE_BYTES] = FORMAT_DECIMAL,
};

/** Layouts of the record types */
static const record_layout_t layouts[RECORD_TYPE_COUNT] = {
    {"S", RECORD_STATE, STATE_FIELD_COUNT, state_formats, store_state},
    {"I", RECORD_INPUT, INPUT_FIELD_COUNT, input_formats, store_input},
    {"O", RECORD_OUTPUT, OUTPUT_FIELD_COUNT, output_formats, store_output},
    {"C", RECORD_CLOCK, CLOCK_FIELD_COUNT, clock_formats, store_clock},
    {"P", RECORD_HANDLER, HANDLER_FIELD_COUNT, handler_formats, store_handler},
    {"R", RECORD_SCANS, SCANS_FIELD_COUNT, scans_formats, store_scans},
    {"H", RECORD_STACK, STACK_FIELD_COUNT, stack_formats, store_stack},
};

// #endregion

// #region Static functions

/**
 * Parses a number field. strtoull() accepts a sign and leading blanks, so the
 * function first checks for a digit.
 *
 * @param rule  Range of the number.
 * @param field Field that gets its value.
 * @param base  strtoull() base of the number.
 * @return true for a number that has only digits and is in the range.
 */
static bool parse_number(const format_rule_t *rule, field_t *field, int base) {
    if (!isxdigit((unsigned char)field->text[0])) {
        return false;
    }
    char *end    = NULL;
    errno        = 0;
    field->value = strtoull(field->text, &end, base);
    if (errno != 0 || *end != '\0' || field->value < rule->minimum) {
        return false;
    }
    return field->value <= rule->maximum;
}

/**
 * Parses a decimal number field.
 *
 * @param rule  Range of the number.
 * @param field Field that gets its value.
 * @return true for a decimal number in the range.
 */
static bool parse_decimal(const format_rule_t *rule, field_t *field) {
    return parse_number(rule, field, DECIMAL_BASE);
}

/**
 * Parses a hexadecimal number field.
 *
 * @param rule  Range of the number.
 * @param field Field that gets its value.
 * @return true for a hexadecimal number in the range.
 */
static bool parse_hex(const format_rule_t *rule, field_t *field) {
    return parse_number(rule, field, HEX_BASE);
}

/**
 * Checks a name field. The field keeps its text, and its value is not used.
 *
 * @param rule  Longest permitted name.
 * @param field Field with the name.
 * @return true for a name of NAME_CHARACTERS that is not empty and not too long.
 */
static bool parse_name(const format_rule_t *rule, field_t *field) {
    size_t length = strlen(field->text);
    if (length == 0 || length > rule->maximum) {
        return false;
    }
    return strspn(field->text, NAME_CHARACTERS) == length;
}

/**
 * Parses a word field.
 *
 * @param rule  Permitted words.
 * @param field Field that gets its value.
 * @return true for a permitted word.
 */
static bool parse_word(const format_rule_t *rule, field_t *field) {
    for (const word_t *word = rule->words; word->text != NULL; word++) {
        if (strcmp(word->text, field->text) == 0) {
            field->value = word->value;
            return true;
        }
    }
    return false;
}

/**
 * Copies the values of a state record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_state(const field_t *fields, record_t *record) {
    record->state = (logging_state_t)fields[STATE_FIELD_STATE].value;
}

/**
 * Copies the values of an input record. The parser checked each maximum, so the
 * casts keep each value.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_input(const field_t *fields, record_t *record) {
    key_event_t *event  = &record->event;
    event->pushed       = fields[INPUT_FIELD_ACTION].value == true;
    event->position.row = (uint8_t)fields[INPUT_FIELD_ROW].value;
    event->position.col = (uint8_t)fields[INPUT_FIELD_COL].value;
    event->layer        = (uint8_t)fields[INPUT_FIELD_LAYER].value;
    event->keycode      = (uint16_t)fields[INPUT_FIELD_KEYCODE].value;
}

/**
 * Copies the values of an output record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_output(const field_t *fields, record_t *record) {
    const field_t *keys = &fields[OUTPUT_FIELD_FIRST_KEY];
    record->report.mods = (uint8_t)fields[OUTPUT_FIELD_MODS].value;
    for (size_t slot = 0; slot < REPORT_KEY_COUNT; slot++) {
        record->report.keys[slot] = (uint8_t)keys[slot].value;
    }
}

/**
 * Copies the values of a clock record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_clock(const field_t *fields, record_t *record) {
    record->cpu_hz = (uint32_t)fields[CLOCK_FIELD_CPU_HZ].value;
}

/**
 * Copies the values of a handler record. The parser checked the name length, so
 * the name fits.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_handler(const field_t *fields, record_t *record) {
    handler_interval_t *handler = &record->handler;
    const char         *name    = fields[HANDLER_FIELD_NAME].text;
    snprintf(handler->name, sizeof(handler->name), "%s", name);
    handler->calls     = (uint32_t)fields[HANDLER_FIELD_CALLS].value;
    handler->cycle_sum = (uint32_t)fields[HANDLER_FIELD_CYCLE_SUM].value;
    handler->cycle_max = (uint32_t)fields[HANDLER_FIELD_CYCLE_MAX].value;
}

/**
 * Copies the values of a scan record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_scans(const field_t *fields, record_t *record) {
    record->scans.scan_count  = (uint32_t)fields[SCANS_FIELD_SCAN_COUNT].value;
    record->scans.interval_ms = (uint32_t)fields[SCANS_FIELD_INTERVAL_MS].value;
}

/**
 * Copies the values of a stack record.
 *
 * @param fields Fields by field position.
 * @param record Record that gets the values.
 */
static void store_stack(const field_t *fields, record_t *record) {
    record->stack_free_bytes = (uint32_t)fields[STACK_FIELD_FREE_BYTES].value;
}

/**
 * Cuts a line into fields at each FIELD_SEPARATOR.
 *
 * @param line   Line that gets a terminator in place of each separator.
 * @param fields Fields that get their text, MAX_FIELD_COUNT items.
 * @return Number of fields, or TOO_MANY_FIELDS.
 */
static size_t split_fields(char *line, field_t *fields) {
    size_t count = 0;
    char  *field = line;
    while (field != NULL && count < MAX_FIELD_COUNT) {
        fields[count++].text = field;
        field                = strchr(field, FIELD_SEPARATOR);
        if (field != NULL) {
            *field++ = '\0';
        }
    }
    return field == NULL ? count : TOO_MANY_FIELDS;
}

/**
 * Parses one field.
 *
 * @param format Format of the field.
 * @param field  Field that gets its value.
 * @return true for a valid field.
 */
static bool parse_field(field_format_t format, field_t *field) {
    const format_rule_t *rule = &format_rules[format];
    return rule->parse(rule, field);
}

/**
 * Finds the layout of a record type letter.
 *
 * @param letter Second field of a record.
 * @return Layout, or NULL for an unknown type.
 */
static const record_layout_t *find_layout(const char *letter) {
    for (size_t index = 0; index < COUNT_OF(layouts); index++) {
        if (strcmp(layouts[index].letter, letter) == 0) {
            return &layouts[index];
        }
    }
    return NULL;
}

/**
 * Finds the layout of a split line. A line with no marker, an unknown type or a
 * different field count gives NULL.
 *
 * @param fields Fields of the line.
 * @param count  Number of fields.
 * @return Layout, or NULL.
 */
static const record_layout_t *match_layout(const field_t *fields, size_t count) {
    if (count <= FIELD_TIME || strcmp(fields[FIELD_MARKER].text, RECORD_MARKER) != 0) {
        return NULL;
    }
    const record_layout_t *layout = find_layout(fields[FIELD_TYPE].text);
    if (layout == NULL || layout->field_count != count) {
        return NULL;
    }
    return layout;
}

/**
 * Skips one status text and its progress characters at the start of a line.
 *
 * @param line Line text.
 * @return Text after the status text, or line if it has no status text.
 */
static char *skip_status_text(char *line) {
    for (size_t index = 0; index < COUNT_OF(status_texts); index++) {
        size_t length = strlen(status_texts[index]);
        if (strncmp(line, status_texts[index], length) == 0) {
            return line + length + strspn(line + length, STATUS_PROGRESS);
        }
    }
    return line;
}

/**
 * Skips all status texts at the start of a line.
 *
 * @param line Line text.
 * @return Text after the status texts.
 */
static char *skip_status_texts(char *line) {
    char *text = line;
    char *next = skip_status_text(text);
    while (next != text) {
        text = next;
        next = skip_status_text(text);
    }
    return text;
}

// #endregion

// #region Public functions

bool parse_record(char *line, record_t *record) {
    field_t                fields[MAX_FIELD_COUNT] = {{.text = NULL}};
    size_t                 count  = split_fields(skip_status_texts(line), fields);
    const record_layout_t *layout = match_layout(fields, count);
    if (layout == NULL) {
        return false;
    }
    for (size_t index = FIELD_TIME; index < count; index++) {
        if (!parse_field(layout->formats[index], &fields[index])) {
            return false;
        }
    }
    record->type = layout->type;
    record->time = (uint32_t)fields[FIELD_TIME].value;
    layout->store(fields, record);
    return true;
}

// #endregion
