/** @file
 * Reads a file into the text that tree-sitter parses, and adds findings at positions
 * in the file. QMK sources contain code that tree-sitter cannot parse directly.
 *
 * An `.inc` file contains the elements of a brace initializer. Thus the checker
 * parses the file in an initializer. See `file_kinds`. Before the parse, the checker
 * replaces the words in `blanked_words` and the lines in `blanked_directives` with
 * spaces.
 */

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "source.h"

/** The characters between the start of a line and a preprocessor directive name. */
#define DIRECTIVE_LEAD "# \t"

/** How the checker parses a file kind: in a wrapper that makes a translation unit. */
typedef struct {
    /** The end of the file names of this kind. */
    const char *suffix;
    /** The text before the file text. */
    const char *before;
    /** The text after the file text. */
    const char *after;
} file_kind_t;

/** The file kinds that need a wrapper. The checker parses all other files directly. */
static const file_kind_t file_kinds[] = {
    {".inc", "static const int inc[] = {\n", "\n};\n"},
};

/** The file kind of a file without a wrapper. */
static const file_kind_t translation_unit = {"", "", ""};

/**
 * Words that tree-sitter cannot parse and that no rule needs. The checker replaces
 * them with spaces before the parse, also in comments and strings. Thus all
 * positions stay the same.
 */
static const char *const blanked_words[] = {
    /* The AVR program memory attribute. tree-sitter reads this macro as a name. */
    "PROGMEM",
};

/**
 * Preprocessor directives whose lines the checker replaces with spaces, as for
 * `blanked_words`.
 */
static const char *const blanked_directives[] = {
    /* tree-sitter parses an include only at a declaration or a statement. The keymap
     * tables include their rows in an initializer. */
    "include",
};

/**
 * Tells if a C string ends with an other string.
 *
 * @param text   The string.
 * @param suffix The end to find.
 * @return True if @p text ends with @p suffix.
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
 * Finds the file kind of a file.
 *
 * @param path The path of the file.
 * @return The row of `file_kinds` whose suffix is the end of @p path, or
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
 * @return The number of newlines in @p text.
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
 * @param byte The byte.
 * @return True if the byte is a letter, a digit or an underscore.
 */
static bool is_word_byte(char byte) {
    return isalnum((unsigned char)byte) != 0 || byte == '_';
}

/**
 * Tells if a text starts with a full word.
 *
 * @param text The NUL-terminated text.
 * @param word The word.
 * @return True if @p text starts with @p word and no word byte follows the word.
 */
static bool starts_with_word(const char *text, const char *word) {
    size_t length = strlen(word);
    if (strncmp(text, word, length) != 0 || is_word_byte(text[length])) {
        return false;
    }
    return true;
}

/**
 * Replaces a word of `blanked_words` with spaces if the word starts at @p at.
 *
 * @param at The start of a word in a NUL-terminated buffer.
 */
static void blank_word(char *at) {
    for (size_t index = 0; index < COUNT_OF(blanked_words); index++) {
        if (starts_with_word(at, blanked_words[index])) {
            memset(at, ' ', strlen(blanked_words[index]));
        }
    }
}

/**
 * Replaces a line with spaces if the line contains a directive of
 * `blanked_directives`.
 *
 * @param line The start of the line in a NUL-terminated buffer.
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
 * Replaces each word of `blanked_words` and each line of `blanked_directives` in a
 * buffer with spaces.
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
 * Reads a file into its parse buffer, in the wrapper of its file kind.
 *
 * @param source The file. It has a path and no text.
 * @return 0, or the errno value of the error that stopped the read.
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
 * @param row    The row in the parse buffer. The first row is 0.
 * @return The line. The first line is FIRST_POSITION. A row of the wrapper gives the
 *         first line.
 */
static unsigned long file_line(const source_t *source, uint32_t row) {
    uint32_t file_row = row < source->wrapper_rows ? 0 : row - source->wrapper_rows;
    return (unsigned long)file_row + FIRST_POSITION;
}

/**
 * Adds a finding without a message.
 *
 * @param source The file of the finding.
 * @param rule   The name of the rule.
 * @param point  The position of the finding in the parse buffer.
 * @return The finding, for describe().
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
