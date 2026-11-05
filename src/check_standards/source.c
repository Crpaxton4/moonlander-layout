/** @file
 * Reading a file into the text that tree-sitter parses, and placing findings in the
 * file: QMK sources hold code that tree-sitter can't parse as it is.
 *
 * An `.inc` file holds the elements of a brace initializer, so it is parsed inside
 * one; see `file_kinds`. The words of `blanked_words` and the lines of
 * `blanked_directives` are parsed as spaces.
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "source.h"

/** Characters between a line's start and a preprocessor directive's name. */
#define DIRECTIVE_LEAD "# \t"

/** How a kind of file is parsed: inside a wrapper that makes it a translation unit. */
typedef struct {
    /** End of the file names of this kind. */
    const char *suffix;
    /** Text parsed before the file's own. */
    const char *before;
    /** Text parsed after the file's own. */
    const char *after;
} file_kind_t;

/** Files parsed inside a wrapper; every other file is parsed as it is. */
static const file_kind_t file_kinds[] = {
    {".inc", "static const int inc[] = {\n", "\n};\n"},
};

/** A file parsed as it is. */
static const file_kind_t translation_unit = {"", "", ""};

/**
 * Words that tree-sitter can't parse and no rule needs, replaced by spaces before
 * parsing, in comments and strings too, so every position stays where it was.
 */
static const char *const blanked_words[] = {
    /* AVR's program memory attribute, a macro that tree-sitter takes for a name */
    "PROGMEM",
};

/**
 * Preprocessor directives whose lines are replaced by spaces before parsing, like
 * `blanked_words`.
 */
static const char *const blanked_directives[] = {
    /* tree-sitter parses an include only where a declaration or a statement can
     * be, not inside an initializer, where the keymap's tables include their rows */
    "include",
};

/**
 * Tells whether a C string ends with another.
 *
 * @param text   The string.
 * @param suffix The ending looked for.
 * @return Whether @p text ends with @p suffix.
 */
static bool ends_with(const char *text, const char *suffix) {
    size_t text_length   = strlen(text);
    size_t suffix_length = strlen(suffix);
    if (suffix_length > text_length) {
        return false;
    }
    return strcmp(text + text_length - suffix_length, suffix) == 0;
}

/**
 * Finds how a file is parsed.
 *
 * @param path Path of the file.
 * @return The row of `file_kinds` whose suffix @p path ends with, or
 *         `translation_unit`.
 */
static const file_kind_t *find_file_kind(const char *path) {
    for (size_t index = 0; index < COUNT_OF(file_kinds); index++) {
        if (ends_with(path, file_kinds[index].suffix)) {
            return &file_kinds[index];
        }
    }
    return &translation_unit;
}

/**
 * Counts the newlines in a C string.
 *
 * @param text The string.
 * @return Number of newlines in @p text.
 */
static uint32_t count_newlines(const char *text) {
    uint32_t newlines = 0;
    for (const char *at = text; *at != '\0'; at++) {
        if (*at == '\n') {
            newlines++;
        }
    }
    return newlines;
}

/**
 * Tells whether a byte can be part of a C identifier.
 *
 * @param byte The byte.
 * @return Whether it is a letter, a digit or an underscore.
 */
static bool is_word_byte(char byte) {
    return isalnum((unsigned char)byte) != 0 || byte == '_';
}

/**
 * Tells whether a text starts with a whole word.
 *
 * @param text The text, NUL-terminated.
 * @param word The word.
 * @return Whether @p text starts with @p word, not followed by a word byte.
 */
static bool starts_with_word(const char *text, const char *word) {
    size_t length = strlen(word);
    if (strncmp(text, word, length) != 0 || is_word_byte(text[length])) {
        return false;
    }
    return true;
}

/**
 * Replaces a word of `blanked_words` with spaces if one starts at a word start.
 *
 * @param at The word start, in a NUL-terminated buffer.
 */
static void blank_word(char *at) {
    for (size_t index = 0; index < COUNT_OF(blanked_words); index++) {
        if (starts_with_word(at, blanked_words[index])) {
            memset(at, ' ', strlen(blanked_words[index]));
        }
    }
}

/**
 * Replaces a line with spaces if it holds a directive of `blanked_directives`.
 *
 * @param line The line start, in a NUL-terminated buffer.
 */
static void blank_directive(char *line) {
    const char *hash = line + strspn(line, BLANKS);
    const char *name = hash + strspn(hash, DIRECTIVE_LEAD);
    if (*hash != '#') {
        return;
    }
    for (size_t index = 0; index < COUNT_OF(blanked_directives); index++) {
        if (starts_with_word(name, blanked_directives[index])) {
            memset(line, ' ', strcspn(line, "\n"));
        }
    }
}

/**
 * Replaces every word of `blanked_words` and every line of `blanked_directives` in
 * a buffer with spaces.
 *
 * @param buffer The buffer.
 */
static void blank_unparsable(buffer_t *buffer) {
    char        previous = '\n';
    const char *end      = buffer->data + buffer->length;
    for (char *at = buffer->data; at < end; at++) {
        if (previous == '\n') {
            blank_directive(at);
        }
        if (!is_word_byte(previous)) {
            blank_word(at);
        }
        previous = *at;
    }
}

/**
 * Reads a file into its parse buffer, inside its file kind's wrapper.
 *
 * @param source The file, with its path set and nothing loaded yet.
 * @return 0, or the errno value of the error that stopped the file being read.
 */
int load_source(source_t *source) {
    const file_kind_t *kind   = find_file_kind(source->path);
    FILE              *stream = fopen(source->path, "rb");
    if (stream == NULL) {
        return errno;
    }
    append(&source->parsed, kind->before);
    source->text_start   = source->parsed.length;
    source->wrapper_rows = count_newlines(kind->before);
    int error            = append_stream(&source->parsed, stream);
    fclose(stream);
    source->text_length = source->parsed.length - source->text_start;
    append(&source->parsed, kind->after);
    blank_unparsable(&source->parsed);
    return error;
}

/**
 * Converts a row of the parse buffer to a line of the file.
 *
 * @param source The file.
 * @param row    Row in the parse buffer, counted from 0.
 * @return The line, counted from FIRST_POSITION; rows of the wrapper count as the
 *         first line.
 */
static unsigned long file_line(const source_t *source, uint32_t row) {
    uint32_t file_row = row < source->wrapper_rows ? 0 : row - source->wrapper_rows;
    return (unsigned long)file_row + FIRST_POSITION;
}

/**
 * Adds a finding without a message.
 *
 * @param source The file the finding is in.
 * @param rule   Name of the rule.
 * @param point  Where the finding is in the parse buffer.
 * @return The finding, for its message.
 */
finding_t *add_finding(source_t *source, const char *rule, TSPoint point) {
    findings_t *list = source->findings;
    list->items = reserve(list->items, sizeof(finding_t), &list->capacity, list->count);
    finding_t *finding = &list->items[list->count];
    *finding           = (finding_t){
        .sequence = list->count,
        .line     = file_line(source, point.row),
        .column   = (unsigned long)point.column + FIRST_POSITION,
        .path     = source->path,
        .rule     = rule,
    };
    list->count++;
    return finding;
}
