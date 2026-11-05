/** @file
 * The checker's rules, and the walk over a file's syntax tree that hands every node
 * to every rule in `rules`.
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
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "rules.h"

/** Deepest nesting of control flow allowed in a function body. */
#define MAX_NESTING_DEPTH 2

/** Most characters of source a parse-error message quotes. */
#define QUOTE_LENGTH 40

/** Opens a Doxygen comment block. */
#define DOC_COMMENT_OPEN "/**"

/** Closes a C comment. */
#define COMMENT_CLOSE "*/"

/** Doxygen command that documents a file. */
#define FILE_COMMAND "@file"

/** Characters that end the file name after FILE_COMMAND. */
#define WHITESPACE " \t\r\n"

/** Bit of the construct kind @p kind in a construct_t `may_contain` mask. */
#define BIT(kind) (1U << (kind))

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
void check_source(TSParser *parser, source_t *source) {
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
