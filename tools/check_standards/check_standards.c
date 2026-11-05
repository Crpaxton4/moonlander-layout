/** @file
 * Standards checker for this repository's C sources: the rules that no established
 * tool checks, run over each file's tree-sitter syntax tree.
 *
 * Usage: `check_standards FILE...`
 *
 * Each file is read and parsed, and one walk over its syntax tree hands every node to
 * every rule in `rules`. Findings are collected for all files and printed once all
 * are checked, sorted by file (argument order), line and column, one per line as
 * `file:line:column rule message`, followed by `N findings in M files`, M being the
 * number of files checked. The exit status is 0 without findings and 1 with any.
 *
 * Rules:
 * - `file-docstring`: the file's very first bytes open a Doxygen comment block that
 *   holds `@file` and a description. Nothing may come before it, not even a blank
 *   line. The word after `@file` on its line is the file name, not the description.
 * - `nesting`: control flow in function bodies nests only as `constructs` allows,
 *   at most MAX_NESTING_DEPTH levels deep.
 * - `parse-error`: tree-sitter found an error or a missing token, so the rules
 *   can't see that part of the file.
 * - `io-error`: the file could not be read.
 *
 * Nesting is judged by tree-sitter node type, inside function bodies only:
 * - Nothing may be nested inside an `if`. An `if` that is all of an `else` continues
 *   its chain at the same level, so `else if` chains are flat.
 * - A `for` may contain `if` and `for`, and `while` and `do` the same, but `while`,
 *   `do` and `switch` only appear at the top level of a function body.
 * - A `switch` contains only its cases, and a case holds no control flow at all.
 * - `?:` counts as an `if`, and `goto` as a branch that can't contain anything;
 *   `return`, `break` and `continue` are exits, allowed anywhere.
 * - A node is inside a construct anywhere in its subtree: an `if` condition counts
 *   as inside the `if`, and a `for` header as inside the `for`.
 * - Compound statements, labels, preprocessor conditionals and every other node
 *   add no level: they pass their enclosing construct on to their children.
 *
 * An `.inc` file holds the elements of a brace initializer, so it is parsed inside
 * one; see `file_kinds`. The words of `blanked_words` and the lines of
 * `blanked_directives` are parsed as spaces.
 */

#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tree_sitter/api.h>

/** Deepest nesting of control flow allowed in a function body. */
#define MAX_NESTING_DEPTH 2

/** Size of a finding's message, terminator included. */
#define MESSAGE_SIZE 160

/** Most characters of source a parse-error message quotes. */
#define QUOTE_LENGTH 40

/** Number of the first line and column in a finding; tree-sitter counts from 0. */
#define FIRST_POSITION 1

/** Index in `argv` of the first file to check. */
#define FIRST_FILE_ARGUMENT 1

/** Bytes read from a file at a time. */
#define READ_CHUNK_SIZE 4096

/** Factor a full array grows by. */
#define GROWTH_FACTOR 2

/** Items a growing array holds beyond twice those in use. */
#define MIN_CAPACITY 16

/** Opens a Doxygen comment block. */
#define DOC_COMMENT_OPEN "/**"

/** Closes a C comment. */
#define COMMENT_CLOSE "*/"

/** Doxygen command that documents a file. */
#define FILE_COMMAND "@file"

/** Characters that separate words on a line. */
#define BLANKS " \t"

/** Characters between a line's start and a preprocessor directive's name. */
#define DIRECTIVE_LEAD "# \t"

/** Characters that end the file name after FILE_COMMAND. */
#define WHITESPACE " \t\r\n"

/** Number of rows in the array @p array. */
#define COUNT_OF(array) (sizeof(array) / sizeof((array)[0]))

/** Bit of the construct kind @p kind in a construct_t `may_contain` mask. */
#define BIT(kind) (1U << (kind))

/**
 * The C grammar, from the tree-sitter-c package's generated parser.
 *
 * @return The grammar, for ts_parser_set_language().
 */
const TSLanguage *tree_sitter_c(void);

/** Control-flow constructs of the nesting rule, each its row in `constructs`. */
typedef enum {
    /** A function body, the outermost level. */
    KIND_FUNCTION,
    /** An `if` statement, `else` branches included. */
    KIND_IF,
    /** A `for` loop. */
    KIND_FOR,
    /** A `while` loop. */
    KIND_WHILE,
    /** A `do` loop. */
    KIND_DO,
    /** A `switch` statement. */
    KIND_SWITCH,
    /** A `case` or `default` label and the statements after it. */
    KIND_CASE,
    /** A conditional expression, the ternary operator. */
    KIND_TERNARY,
    /** A `goto` statement. */
    KIND_GOTO,
    /** Number of construct kinds. */
    KIND_COUNT,
} construct_kind_t;

/** What may be nested directly inside a construct that holds no control flow. */
#define NOTHING 0U

/** What a `switch` may contain directly: its cases. */
#define CASES BIT(KIND_CASE)

/** What a loop body may contain directly: one level of branches and `for` loops. */
#define LOOP_BODY (BIT(KIND_IF) | BIT(KIND_FOR) | BIT(KIND_TERNARY) | BIT(KIND_GOTO))

/** What a function body may contain directly: a loop body plus the top-level kinds. */
#define FUNCTION_BODY (LOOP_BODY | BIT(KIND_WHILE) | BIT(KIND_DO) | BIT(KIND_SWITCH))

/** A control-flow construct of the nesting rule and what may be nested inside it. */
typedef struct {
    /** tree-sitter node type of the construct. */
    const char *node_type;
    /** Name of the construct in messages. */
    const char *label;
    /**
     * Parent node type under which the construct continues the construct around
     * that parent instead of nesting in it, or NULL.
     */
    const char *continues_under;
    /** BIT() mask of the constructs allowed directly inside this one. */
    unsigned may_contain;
    /** Nesting levels the construct adds: 1, or 0 for a function body or a case. */
    int levels;
} construct_t;

/** The nesting rule's constructs, indexed by construct_kind_t. */
static const construct_t constructs[KIND_COUNT] = {
    [KIND_FUNCTION] = {"function_definition", "function", NULL, FUNCTION_BODY, 0},
    [KIND_IF]       = {"if_statement", "if", "else_clause", NOTHING, 1},
    [KIND_FOR]      = {"for_statement", "for", NULL, LOOP_BODY, 1},
    [KIND_WHILE]    = {"while_statement", "while", NULL, LOOP_BODY, 1},
    [KIND_DO]       = {"do_statement", "do", NULL, LOOP_BODY, 1},
    [KIND_SWITCH]   = {"switch_statement", "switch", NULL, CASES, 1},
    [KIND_CASE]     = {"case_statement", "case", NULL, NOTHING, 0},
    [KIND_TERNARY]  = {"conditional_expression", "?:", NULL, NOTHING, 1},
    [KIND_GOTO]     = {"goto_statement", "goto", NULL, NOTHING, 1},
};

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

/** A growing block of bytes, NUL-terminated after each append(). */
typedef struct {
    /** The bytes. */
    char *data;
    /** Bytes in use, the terminator excluded. */
    size_t length;
    /** Bytes allocated. */
    size_t capacity;
} buffer_t;

/** One finding, as printed. */
typedef struct {
    /** Order in which the finding was reported, the last sort key. */
    size_t sequence;
    /** Line, counted from FIRST_POSITION. */
    unsigned long line;
    /** Column in bytes, counted from FIRST_POSITION. */
    unsigned long column;
    /** Path of the file as given. */
    const char *path;
    /** Name of the rule. */
    const char *rule;
    /** What is wrong. */
    char message[MESSAGE_SIZE];
} finding_t;

/** Every finding so far. */
typedef struct {
    /** The findings, in file order, each file's sorted once it is checked. */
    finding_t *items;
    /** Findings in use. */
    size_t count;
    /** Findings allocated. */
    size_t capacity;
} findings_t;

/** A file being checked. */
typedef struct {
    /** Path of the file as given. */
    const char *path;
    /** What tree-sitter parses: the file's text inside its kind's wrapper. */
    buffer_t parsed;
    /** Offset of the file's own text in `parsed`. */
    size_t text_start;
    /** Length of the file's own text. */
    size_t text_length;
    /** Lines the wrapper adds before the file's text. */
    uint32_t wrapper_rows;
    /** Where findings go. */
    findings_t *findings;
} source_t;

/** Where a node sits in a function body. */
typedef struct {
    /** Innermost construct around the node, or NULL outside function bodies. */
    const construct_t *construct;
    /** Nesting levels around the node. */
    int depth;
} scope_t;

/** What a walk knows about a node's parent. */
typedef struct {
    /** File the nodes belong to. */
    source_t *source;
    /** Node type of the parent, "" for the root. */
    const char *type;
    /** Scope of the parent's children. */
    scope_t scope;
} parent_t;

/** One node of a walk, as the rules see it. */
typedef struct {
    /** File the node belongs to. */
    source_t *source;
    /** The node. */
    TSNode node;
    /** Construct the node opens, or NULL if it opens none. */
    const construct_t *opens;
    /** Scope the node sits in. */
    scope_t scope;
} visit_t;

/**
 * Judges one node for one rule and reports what is wrong with it.
 *
 * @param rule  Name of the rule, for findings.
 * @param visit The node and its context.
 */
typedef void rule_visitor_t(const char *rule, const visit_t *visit);

/** A rule: its name and the visitor every node of a walk is handed to. */
typedef struct {
    /** Name of the rule in findings. */
    const char *name;
    /** Judges each node. */
    rule_visitor_t *visit;
} rule_t;

/** Docstring of a file: the Doxygen comment its first bytes open, if any. */
typedef struct {
    /** First byte after the opening of the comment, or NULL if there is none. */
    const char *body;
    /** The closing of the comment, or NULL if there is none. */
    const char *body_end;
    /** The comment's first FILE_COMMAND, or NULL if there is none. */
    const char *command;
} docstring_t;

/** One requirement of the file-docstring rule and the message if it isn't met. */
typedef struct {
    /**
     * Tells whether a docstring meets the requirement.
     *
     * @param docstring The docstring, which meets every earlier requirement.
     * @return Whether it meets this one.
     */
    bool (*passes)(const docstring_t *docstring);
    /** Message when it doesn't. */
    const char *message;
} docstring_check_t;

/**
 * Makes room in an array for at least one item after its first @p count.
 *
 * @param items     The array, or NULL if none is allocated yet.
 * @param item_size Size of one item.
 * @param capacity  Items allocated; updated when the array grows.
 * @param count     Items in use.
 * @return The array, moved if it grew.
 */
static void *reserve(void *items, size_t item_size, size_t *capacity, size_t count) {
    if (count < *capacity) {
        return items;
    }
    *capacity   = count * GROWTH_FACTOR + MIN_CAPACITY;
    void *grown = realloc(items, *capacity * item_size);
    if (grown == NULL) {
        perror("check_standards");
        abort();
    }
    return grown;
}

/**
 * Makes room in a buffer for @p extra more bytes and the terminator.
 *
 * @param buffer The buffer.
 * @param extra  Bytes to add after its current length.
 */
static void reserve_bytes(buffer_t *buffer, size_t extra) {
    buffer->data =
        reserve(buffer->data, sizeof(char), &buffer->capacity, buffer->length + extra);
}

/**
 * Appends a C string to a buffer.
 *
 * @param buffer The buffer.
 * @param text   The string.
 */
static void append(buffer_t *buffer, const char *text) {
    size_t length = strlen(text);
    reserve_bytes(buffer, length);
    memcpy(buffer->data + buffer->length, text, length);
    buffer->length += length;
    buffer->data[buffer->length] = '\0';
}

/**
 * Appends everything left in a stream to a buffer, without the terminator.
 *
 * @param buffer The buffer.
 * @param stream The stream.
 * @return 0, or the errno value of a read error.
 */
static int append_stream(buffer_t *buffer, FILE *stream) {
    size_t read_size = 0;
    do {
        reserve_bytes(buffer, READ_CHUNK_SIZE);
        read_size =
            fread(buffer->data + buffer->length, sizeof(char), READ_CHUNK_SIZE, stream);
        buffer->length += read_size;
    } while (read_size == READ_CHUNK_SIZE);
    if (ferror(stream) != 0) {
        return errno;
    }
    return 0;
}

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
static int load_source(source_t *source) {
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
static finding_t *add_finding(source_t *source, const char *rule, TSPoint point) {
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

/**
 * Writes the message of a finding.
 *
 * @param finding The finding.
 * @param format  printf() format of the message, followed by its arguments.
 */
static void describe(finding_t *finding, const char *format, ...)
    __attribute__((format(printf, 2, 3)));

static void describe(finding_t *finding, const char *format, ...) {
    va_list arguments;
    va_start(arguments, format);
    vsnprintf(finding->message, sizeof(finding->message), format, arguments);
    va_end(arguments);
}

/**
 * Finds a C string in a range of bytes.
 *
 * @param start  Start of the range.
 * @param end    End of the range.
 * @param needle The string looked for.
 * @return The first byte of the first @p needle in the range, or NULL.
 */
static const char *find_text(const char *start, const char *end, const char *needle) {
    size_t length = strlen(needle);
    for (const char *at = start; (size_t)(end - at) >= length; at++) {
        if (memcmp(at, needle, length) == 0) {
            return at;
        }
    }
    return NULL;
}

/**
 * Tells whether a range of bytes holds a letter or a digit.
 *
 * @param start Start of the range.
 * @param end   End of the range.
 * @return Whether any byte of the range is alphanumeric.
 */
static bool has_alphanumeric(const char *start, const char *end) {
    for (const char *at = start; at < end; at++) {
        if (isalnum((unsigned char)*at) != 0) {
            return true;
        }
    }
    return false;
}

/**
 * Finds the docstring at the start of a file.
 *
 * @param text   The file's text, followed by a NUL.
 * @param length Length of the text.
 * @return The docstring; `body` is NULL if the text doesn't begin with one.
 */
static docstring_t find_docstring(const char *text, size_t length) {
    docstring_t docstring   = {.body = NULL, .body_end = NULL, .command = NULL};
    size_t      open_length = strlen(DOC_COMMENT_OPEN);
    if (length < open_length || memcmp(text, DOC_COMMENT_OPEN, open_length) != 0 ||
        text[open_length] == '/') {
        return docstring;
    }
    docstring.body     = text + open_length;
    docstring.body_end = find_text(docstring.body, text + length, COMMENT_CLOSE);
    if (docstring.body_end != NULL) {
        docstring.command = find_text(docstring.body, docstring.body_end, FILE_COMMAND);
    }
    return docstring;
}

/**
 * Tells whether a file begins with a Doxygen comment.
 *
 * @param docstring The file's docstring.
 * @return Whether it was found.
 */
static bool opens_doc_comment(const docstring_t *docstring) {
    return docstring->body != NULL;
}

/**
 * Tells whether a docstring is closed.
 *
 * @param docstring The docstring.
 * @return Whether its closing was found.
 */
static bool is_closed(const docstring_t *docstring) {
    return docstring->body_end != NULL;
}

/**
 * Tells whether a docstring holds FILE_COMMAND.
 *
 * @param docstring The docstring.
 * @return Whether it does.
 */
static bool has_file_command(const docstring_t *docstring) {
    return docstring->command != NULL;
}

/**
 * Tells whether a docstring describes its file: whether it holds a letter or a
 * digit outside FILE_COMMAND and the file name after it on its line.
 *
 * @param docstring The docstring, which holds FILE_COMMAND.
 * @return Whether it does.
 */
static bool has_description(const docstring_t *docstring) {
    const char *name = docstring->command + strlen(FILE_COMMAND);
    name += strspn(name, BLANKS);
    if (has_alphanumeric(docstring->body, docstring->command)) {
        return true;
    }
    return has_alphanumeric(name + strcspn(name, WHITESPACE), docstring->body_end);
}

/** The file-docstring rule's requirements, in order: each needs the earlier ones. */
static const docstring_check_t docstring_checks[] = {
    {opens_doc_comment, "file does not begin with a /** comment"},
    {is_closed, "the opening /** comment is not closed"},
    {has_file_command, "the opening /** comment has no @file"},
    {has_description, "the @file comment has no description"},
};

/**
 * Rule file-docstring: reports, at the root node, the first requirement of
 * `docstring_checks` that the file's docstring doesn't meet.
 *
 * @param rule  Name of the rule.
 * @param visit The node.
 */
static void check_file_docstring(const char *rule, const visit_t *visit) {
    if (strcmp(ts_node_type(visit->node), "translation_unit") != 0) {
        return;
    }
    source_t   *source    = visit->source;
    const char *text      = source->parsed.data + source->text_start;
    docstring_t docstring = find_docstring(text, source->text_length);
    TSPoint     start     = {.row = source->wrapper_rows, .column = 0};
    for (size_t index = 0; index < COUNT_OF(docstring_checks); index++) {
        if (!docstring_checks[index].passes(&docstring)) {
            finding_t *finding = add_finding(source, rule, start);
            describe(finding, "%s", docstring_checks[index].message);
            return;
        }
    }
}

/**
 * Bit of a construct in a construct_t `may_contain` mask.
 *
 * @param construct The construct, a row of `constructs`.
 * @return Its BIT().
 */
static unsigned construct_bit(const construct_t *construct) {
    return BIT((unsigned)(construct - constructs));
}

/**
 * Rule nesting: reports a construct that its enclosing construct may not contain,
 * or that nests deeper than MAX_NESTING_DEPTH.
 *
 * @param rule  Name of the rule.
 * @param visit The node.
 */
static void check_nesting(const char *rule, const visit_t *visit) {
    const construct_t *inner = visit->opens;
    const construct_t *outer = visit->scope.construct;
    if (inner == NULL || outer == NULL) {
        return;
    }
    int     depth = visit->scope.depth + inner->levels;
    TSPoint start = ts_node_start_point(visit->node);
    if ((outer->may_contain & construct_bit(inner)) == 0) {
        finding_t *finding = add_finding(visit->source, rule, start);
        describe(finding, "%s is not allowed inside %s", inner->label, outer->label);
        return;
    }
    if (depth > MAX_NESTING_DEPTH) {
        finding_t *finding = add_finding(visit->source, rule, start);
        describe(finding, "%s nested deeper than %d", inner->label, MAX_NESTING_DEPTH);
    }
}

/**
 * Length of the quote of a source fragment: up to its first line end, at most
 * QUOTE_LENGTH.
 *
 * @param text The fragment, NUL-terminated.
 * @return Characters to quote.
 */
static int quote_length(const char *text) {
    size_t length = strcspn(text, "\r\n");
    return (int)(length < QUOTE_LENGTH ? length : QUOTE_LENGTH);
}

/**
 * Rule parse-error: reports every error and every missing token tree-sitter found.
 *
 * @param rule  Name of the rule.
 * @param visit The node.
 */
static void check_parse_error(const char *rule, const visit_t *visit) {
    TSNode      node  = visit->node;
    TSPoint     start = ts_node_start_point(node);
    const char *text  = visit->source->parsed.data + ts_node_start_byte(node);
    if (ts_node_is_missing(node)) {
        finding_t *finding = add_finding(visit->source, rule, start);
        describe(finding, "missing `%s`", ts_node_type(node));
    }
    if (strcmp(ts_node_type(node), "ERROR") == 0) {
        finding_t *finding = add_finding(visit->source, rule, start);
        describe(finding, "cannot parse `%.*s`", quote_length(text), text);
    }
}

/** Every rule, in the order each node is handed to them. */
static const rule_t rules[] = {
    {"file-docstring", check_file_docstring},
    {"nesting", check_nesting},
    {"parse-error", check_parse_error},
};

/**
 * Finds the construct of a node type.
 *
 * @param node_type tree-sitter node type.
 * @return The row of `constructs` for @p node_type, or NULL.
 */
static const construct_t *find_construct(const char *node_type) {
    for (size_t kind = 0; kind < KIND_COUNT; kind++) {
        if (strcmp(constructs[kind].node_type, node_type) == 0) {
            return &constructs[kind];
        }
    }
    return NULL;
}

/**
 * Tells whether a construct continues the one around its parent, as `else if`
 * does.
 *
 * @param construct   The construct.
 * @param parent_type Node type of its parent.
 * @return Whether @p parent_type is the construct's `continues_under`.
 */
static bool continues_chain(const construct_t *construct, const char *parent_type) {
    if (construct->continues_under == NULL) {
        return false;
    }
    return strcmp(construct->continues_under, parent_type) == 0;
}

/**
 * Finds the construct a node opens.
 *
 * @param node   The node.
 * @param parent The node's parent.
 * @return The node's construct, or NULL if it is none, continues a chain, or lies
 *         outside function bodies.
 */
static const construct_t *opened_construct(TSNode node, const parent_t *parent) {
    const construct_t *construct = find_construct(ts_node_type(node));
    if (construct == NULL || continues_chain(construct, parent->type)) {
        return NULL;
    }
    if (parent->scope.construct == NULL && construct != &constructs[KIND_FUNCTION]) {
        return NULL;
    }
    return construct;
}

/**
 * Scope of a node's children.
 *
 * @param scope     Scope of the node.
 * @param construct Construct the node opens, or NULL.
 * @return @p scope, or the scope inside @p construct.
 */
static scope_t enter(scope_t scope, const construct_t *construct) {
    if (construct == NULL) {
        return scope;
    }
    return (scope_t){.construct = construct, .depth = scope.depth + construct->levels};
}

/**
 * Hands a node and, depth first, every node below it to every rule.
 *
 * @param cursor Cursor on the node; back on it on return.
 * @param parent The node's parent.
 */
static void walk(TSTreeCursor *cursor, const parent_t *parent) {
    TSNode             node  = ts_tree_cursor_current_node(cursor);
    const construct_t *opens = opened_construct(node, parent);
    visit_t            visit = {parent->source, node, opens, parent->scope};
    for (size_t index = 0; index < COUNT_OF(rules); index++) {
        rules[index].visit(rules[index].name, &visit);
    }
    if (!ts_tree_cursor_goto_first_child(cursor)) {
        return;
    }
    scope_t  inner    = enter(parent->scope, opens);
    parent_t children = {parent->source, ts_node_type(node), inner};
    do {
        walk(cursor, &children);
    } while (ts_tree_cursor_goto_next_sibling(cursor));
    ts_tree_cursor_goto_parent(cursor);
}

/**
 * Reads, parses and checks one file.
 *
 * @param parser The parser.
 * @param source The file, with nothing loaded yet.
 */
static void check_source(TSParser *parser, source_t *source) {
    int error = load_source(source);
    if (error != 0) {
        TSPoint start = {.row = 0, .column = 0};
        describe(add_finding(source, "io-error", start), "%s", strerror(error));
        return;
    }
    uint32_t length = (uint32_t)source->parsed.length;
    TSTree  *tree   = ts_parser_parse_string(parser, NULL, source->parsed.data, length);
    TSTreeCursor cursor = ts_tree_cursor_new(ts_tree_root_node(tree));
    parent_t     root   = {source, "", {.construct = NULL, .depth = 0}};
    walk(&cursor, &root);
    ts_tree_cursor_delete(&cursor);
    ts_tree_delete(tree);
}

/**
 * Orders two numbers for qsort().
 *
 * @param left  The first number.
 * @param right The second number.
 * @return Negative, zero or positive as @p left is less than, equal to or greater
 *         than @p right.
 */
static int compare_numbers(size_t left, size_t right) {
    return (left > right) - (left < right);
}

/**
 * Orders two findings of one file by line, column and reporting order, for qsort().
 *
 * @param first  The first finding.
 * @param second The second finding.
 * @return Negative, zero or positive as @p first goes before, with or after
 *         @p second.
 */
static int compare_findings(const void *first, const void *second) {
    const finding_t *one   = first;
    const finding_t *other = second;
    int              order = compare_numbers(one->line, other->line);
    order = order != 0 ? order : compare_numbers(one->column, other->column);
    return order != 0 ? order : compare_numbers(one->sequence, other->sequence);
}

/**
 * Checks the file at a path and sorts its findings.
 *
 * @param parser   The parser.
 * @param findings Where findings go.
 * @param path     Path of the file.
 */
static void check_path(TSParser *parser, findings_t *findings, const char *path) {
    size_t   first  = findings->count;
    source_t source = {.path = path, .findings = findings};
    check_source(parser, &source);
    free(source.parsed.data);
    if (findings->count > first) {
        size_t count = findings->count - first;
        qsort(findings->items + first, count, sizeof(finding_t), compare_findings);
    }
}

/**
 * Prints every finding and the summary line.
 *
 * @param findings   The findings.
 * @param file_count Number of files checked.
 */
static void print_findings(const findings_t *findings, int file_count) {
    for (size_t index = 0; index < findings->count; index++) {
        const finding_t *finding = &findings->items[index];
        printf("%s:%lu:%lu ", finding->path, finding->line, finding->column);
        printf("%s %s\n", finding->rule, finding->message);
    }
    printf("%zu findings in %d files\n", findings->count, file_count);
}

/**
 * Checks every file named on the command line, then prints the findings.
 *
 * @param argc Number of arguments, the program name included.
 * @param argv The program name, then the paths of the files to check.
 * @return EXIT_SUCCESS without findings, EXIT_FAILURE with any.
 */
int main(int argc, char *argv[]) {
    TSParser *parser = ts_parser_new();
    if (!ts_parser_set_language(parser, tree_sitter_c())) {
        fputs("check_standards: tree-sitter can't load the C grammar\n", stderr);
        return EXIT_FAILURE;
    }
    findings_t findings = {.items = NULL, .count = 0, .capacity = 0};
    for (int index = FIRST_FILE_ARGUMENT; index < argc; index++) {
        check_path(parser, &findings, argv[index]);
    }
    ts_parser_delete(parser);
    print_findings(&findings, argc - FIRST_FILE_ARGUMENT);
    free(findings.items);
    return findings.count == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
