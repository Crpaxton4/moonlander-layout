/** @file
 * The rules of the checker, and the walk that gives each node of a syntax tree to
 * each rule in `rules`.
 *
 * Rules:
 * - `file-docstring`: the first bytes of the file open a Doxygen comment block that
 *   contains `@file` and a description. Nothing can come before the block, also no
 *   blank line. The word after `@file` on the same line is the file name, not the
 *   description.
 * - `nesting`: control flow in function bodies nests only as `constructs` allows,
 *   to a maximum of MAX_NESTING_DEPTH levels.
 * - `parse-error`: tree-sitter found an error or a missing token. The rules cannot
 *   examine that part of the file.
 * - `io-error`: the checker cannot read the file.
 *
 * The nesting rule uses the tree-sitter node type, and only in function bodies:
 * - Nothing can nest in an `if`. An `if` that is the full body of an `else`
 *   continues its chain at the same level. Thus `else if` chains are flat.
 * - A `for`, a `while` and a `do` can contain an `if` and a `for`. A `while`, a `do`
 *   and a `switch` can only be at the top level of a function body.
 * - A `switch` contains only its cases. A case contains no control flow.
 * - `?:` counts as an `if`, and `goto` as a branch that cannot contain anything.
 *   The exits `return`, `break` and `continue` are allowed at all levels.
 * - A construct contains its full subtree: an `if` condition is in the `if`, and a
 *   `for` header is in the `for`.
 * - Compound statements, labels, preprocessor conditionals and all other nodes add
 *   no level. Their children are in the same construct as the node.
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "rules.h"

/** The maximum nesting depth of control flow in a function body. */
#define MAX_NESTING_DEPTH 2

/** The maximum number of source characters that a parse-error message quotes. */
#define QUOTE_LENGTH 40

/** The start of a Doxygen comment block. */
#define DOC_COMMENT_OPEN "/**"

/** The end of a C comment. */
#define COMMENT_CLOSE "*/"

/** The Doxygen command that documents a file. */
#define FILE_COMMAND "@file"

/** The characters that end the file name after FILE_COMMAND. */
#define WHITESPACE " \t\r\n"

/** The bit of the construct kind @p kind in a `may_contain` mask of construct_t. */
#define BIT(kind) (1U << (kind))

/** The constructs of the nesting rule. Each value is its row index in `constructs`. */
typedef enum {
    /** A function body, the outermost level. */
    KIND_FUNCTION,
    /** An `if` statement, with its `else` branches. */
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
    /** A conditional expression (the ternary operator). */
    KIND_TERNARY,
    /** A `goto` statement. */
    KIND_GOTO,
    /** The number of construct kinds. */
    KIND_COUNT,
} construct_kind_t;

/** The mask of a construct that contains no control flow. */
#define NOTHING 0U

/** The mask of a `switch`: only its cases. */
#define CASES BIT(KIND_CASE)

/** The mask of a loop body: one level of branches and `for` loops. */
#define LOOP_BODY (BIT(KIND_IF) | BIT(KIND_FOR) | BIT(KIND_TERNARY) | BIT(KIND_GOTO))

/** The mask of a function body: the loop body kinds and the top-level kinds. */
#define FUNCTION_BODY (LOOP_BODY | BIT(KIND_WHILE) | BIT(KIND_DO) | BIT(KIND_SWITCH))

/** A construct of the nesting rule, and the constructs that it can contain. */
typedef struct {
    /** The tree-sitter node type of the construct. */
    const char *node_type;
    /** The name of the construct in messages. */
    const char *label;
    /**
     * A parent node type, or NULL. Under this parent, the construct continues the
     * construct around the parent. It does not nest in it.
     */
    const char *continues_under;
    /** The BIT() mask of the constructs that can be directly in this construct. */
    unsigned may_contain;
    /** The number of levels that the construct adds: 1, or 0 for a body or a case. */
    int levels;
} construct_t;

/** The constructs of the nesting rule. construct_kind_t is the index. */
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

/** The position of a node in a function body. */
typedef struct {
    /** The innermost construct around the node, or NULL outside function bodies. */
    const construct_t *construct;
    /** The number of nesting levels around the node. */
    int depth;
} scope_t;

/** The data that the walk has about the parent of a node. */
typedef struct {
    /** The file of the nodes. */
    source_t *source;
    /** The node type of the parent, or "" for the root. */
    const char *type;
    /** The scope of the children of the parent. */
    scope_t scope;
} parent_t;

/** One node of the walk, as the rules see it. */
typedef struct {
    /** The file of the node. */
    source_t *source;
    /** The node. */
    TSNode node;
    /** The construct that the node opens, or NULL. */
    const construct_t *opens;
    /** The scope of the node. */
    scope_t scope;
} visit_t;

/**
 * Examines one node for one rule and reports each problem.
 *
 * @param rule  The name of the rule, for findings.
 * @param visit The node and its context.
 */
typedef void rule_visitor_t(const char *rule, const visit_t *visit);

/** A rule: its name and the visitor that gets each node of the walk. */
typedef struct {
    /** The name of the rule in findings. */
    const char *name;
    /** The function that examines each node. */
    rule_visitor_t *visit;
} rule_t;

/** The docstring of a file: the Doxygen comment that its first bytes open, if any. */
typedef struct {
    /** The first byte after the comment start, or NULL if there is no comment. */
    const char *body;
    /** The comment end, or NULL if there is no end. */
    const char *body_end;
    /** The first FILE_COMMAND in the comment, or NULL if there is none. */
    const char *command;
} docstring_t;

/** One requirement of the file-docstring rule, and its message. */
typedef struct {
    /**
     * Tells if a docstring meets the requirement.
     *
     * @param docstring The docstring. It meets all earlier requirements.
     * @return True if it meets this requirement.
     */
    bool (*passes)(const docstring_t *docstring);
    /** The message if the docstring does not meet the requirement. */
    const char *message;
} docstring_check_t;

/**
 * Finds a C string in a range of bytes.
 *
 * @param start  The start of the range.
 * @param end    The end of the range.
 * @param needle The string to find.
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
 * Tells if a range of bytes contains a letter or a digit.
 *
 * @param start The start of the range.
 * @param end   The end of the range.
 * @return True if a byte of the range is a letter or a digit.
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
 * @param text   The text of the file, with a NUL after it.
 * @param length The length of the text.
 * @return The docstring. Its `body` is NULL if the text does not start with one.
 */
static docstring_t find_docstring(const char *text, size_t length) {
    docstring_t docstring   = {.body = NULL, .body_end = NULL, .command = NULL};
    size_t      open_length = strlen(DOC_COMMENT_OPEN);
    /* A slash after DOC_COMMENT_OPEN closes the comment at once. That empty comment
     * is not a Doxygen block. */
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
 * Tells if a file starts with a Doxygen comment.
 *
 * @param docstring The docstring of the file.
 * @return True if find_docstring() found it.
 */
static bool opens_doc_comment(const docstring_t *docstring) {
    return docstring->body != NULL;
}

/**
 * Tells if a docstring has an end.
 *
 * @param docstring The docstring.
 * @return True if it has an end.
 */
static bool is_closed(const docstring_t *docstring) {
    return docstring->body_end != NULL;
}

/**
 * Tells if a docstring contains FILE_COMMAND.
 *
 * @param docstring The docstring.
 * @return True if it contains FILE_COMMAND.
 */
static bool has_file_command(const docstring_t *docstring) {
    return docstring->command != NULL;
}

/**
 * Tells if a docstring describes its file. A description is a letter or a digit
 * that is not FILE_COMMAND or the file name after it on the same line.
 *
 * @param docstring The docstring. It contains FILE_COMMAND.
 * @return True if it has a description.
 */
static bool has_description(const docstring_t *docstring) {
    const char *name = docstring->command + strlen(FILE_COMMAND);
    name += strspn(name, BLANKS);
    if (has_alphanumeric(docstring->body, docstring->command)) {
        return true;
    }
    return has_alphanumeric(name + strcspn(name, WHITESPACE), docstring->body_end);
}

/** The requirements of the file-docstring rule. Each one needs the ones before it. */
static const docstring_check_t docstring_checks[] = {
    {opens_doc_comment, "file does not begin with a /** comment"},
    {is_closed, "the opening /** comment is not closed"},
    {has_file_command, "the opening /** comment has no @file"},
    {has_description, "the @file comment has no description"},
};

/**
 * Rule file-docstring. At the root node, it reports the first requirement in
 * `docstring_checks` that the docstring of the file does not meet.
 *
 * @param rule  The name of the rule.
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
 * Gets the bit of a construct in a `may_contain` mask of construct_t.
 *
 * @param construct The construct, a row of `constructs`.
 * @return Its BIT().
 */
static unsigned construct_bit(const construct_t *construct) {
    return BIT((unsigned)(construct - constructs));
}

/**
 * Rule nesting. It reports a construct that its outer construct cannot contain, or
 * that nests deeper than MAX_NESTING_DEPTH.
 *
 * @param rule  The name of the rule.
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
 * Gets the length of the quote of a source fragment. The quote stops at the first
 * line end, with a maximum of QUOTE_LENGTH.
 *
 * @param text The NUL-terminated fragment.
 * @return The number of characters to quote.
 */
static int quote_length(const char *text) {
    size_t length = strcspn(text, "\r\n");
    return (int)(length < QUOTE_LENGTH ? length : QUOTE_LENGTH);
}

/**
 * Rule parse-error. It reports each error and each missing token that tree-sitter
 * found.
 *
 * @param rule  The name of the rule.
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

/** All rules, in the order in which each node goes to them. */
static const rule_t rules[] = {
    {"file-docstring", check_file_docstring},
    {"nesting", check_nesting},
    {"parse-error", check_parse_error},
};

/**
 * Finds the construct of a node type.
 *
 * @param node_type The tree-sitter node type.
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
 * Tells if a construct continues the construct around its parent, as `else if`
 * does.
 *
 * @param construct   The construct.
 * @param parent_type The node type of its parent.
 * @return True if @p parent_type is the `continues_under` of the construct.
 */
static bool continues_chain(const construct_t *construct, const char *parent_type) {
    if (construct->continues_under == NULL) {
        return false;
    }
    return strcmp(construct->continues_under, parent_type) == 0;
}

/**
 * Finds the construct that a node opens.
 *
 * @param node   The node.
 * @param parent The parent of the node.
 * @return The construct of the node. NULL if the node is not a construct, continues
 *         a chain, or is outside function bodies.
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
 * Gets the scope of the children of a node.
 *
 * @param scope     The scope of the node.
 * @param construct The construct that the node opens, or NULL.
 * @return @p scope, or the scope in @p construct.
 */
static scope_t enter(scope_t scope, const construct_t *construct) {
    if (construct == NULL) {
        return scope;
    }
    return (scope_t){.construct = construct, .depth = scope.depth + construct->levels};
}

/**
 * Gives a node and all nodes below it to each rule, depth first.
 *
 * @param cursor A cursor on the node. On return, the cursor is on the node again.
 * @param parent The parent of the node.
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
 * @param source The file. It has a path and no text.
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
