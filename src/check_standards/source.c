/** @file
 * Reads a file into the text that tree-sitter parses
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "source.h"

/** Characters before the name of a preprocessor directive */
#define DIRECTIVE_LEAD "# \t"

/** Wrapper that makes a file of one kind into a translation unit */
typedef struct {
    const char *suffix; /**< End of the file name */
    const char *before; /**< Text before the file text */
    const char *after;  /**< Text after the file text */
} file_kind_t;

/** An .inc file contains only the elements of a brace initializer */
static const file_kind_t file_kinds[] = {
    {".inc", "static const int inc[] = {\n", "\n};\n"},
};

/** File kind of each file that file_kinds does not match */
static const file_kind_t translation_unit = {"", "", ""};

/**
 * Spaces replace these words before the parse, also in comments and strings, so
 * all positions stay the same.
 */
static const char *const blanked_words[] = {
    // tree-sitter reads this AVR attribute macro as a name
    "PROGMEM",
};

/** Spaces replace the lines of these directives before the parse */
static const char *const blanked_directives[] = {
    // tree-sitter cannot parse an include in an initializer, where the keymap
    // tables include their rows
    "include",
};

/**
 * Tells if a string ends with a suffix.
 *
 * @param text   String to examine.
 * @param suffix Possible end of text.
 * @return True if text ends with suffix.
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
 * Finds the file kind of a path from its suffix.
 *
 * @param path File path.
 * @return Matching file kind, or translation_unit.
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
 * Counts the newline characters of a string.
 *
 * @param text NUL-terminated string.
 * @return Number of newlines.
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
 * Tells if a byte can be part of a C identifier.
 *
 * @param byte Byte to examine.
 * @return True for a letter, a digit or an underscore.
 */
static bool is_word_byte(char byte) {
    return isalnum((unsigned char)byte) != 0 || byte == '_';
}

/**
 * Tells if a string starts with a full word, and not with only the start of a
 * longer word.
 *
 * @param text String to examine.
 * @param word Word to find.
 * @return True if text starts with word and no word byte follows it.
 */
static bool starts_with_word(const char *text, const char *word) {
    size_t length = strlen(word);
    if (strncmp(text, word, length) != 0 || is_word_byte(text[length])) {
        return false;
    }
    return true;
}

/**
 * Replaces a word of blanked_words with spaces if the text starts with it.
 *
 * @param at Text at the start of a possible word.
 */
static void blank_word(char *at) {
    for (size_t index = 0; index < COUNT_OF(blanked_words); index++) {
        if (starts_with_word(at, blanked_words[index])) {
            memset(at, ' ', strlen(blanked_words[index]));
        }
    }
}

/**
 * Replaces a line with spaces if it is a directive of blanked_directives. The
 * newline stays.
 *
 * @param line Text at the start of a line.
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
 * Replaces the words and directives that tree-sitter cannot parse with spaces.
 *
 * @param buffer Parsed text of a source.
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
 * Changes a row of the parsed text into a line of the file. A row of the wrapper
 * gives the first line of the file.
 *
 * @param source Source of the row.
 * @param row    Row in the parsed text, from 0.
 * @return Line in the file, from FIRST_POSITION.
 */
static unsigned long file_line(const source_t *source, uint32_t row) {
    uint32_t file_row = row < source->wrapper_rows ? 0 : row - source->wrapper_rows;
    return (unsigned long)file_row + FIRST_POSITION;
}

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
