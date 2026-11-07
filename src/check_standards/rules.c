/** @file
 * Checker rules and the walk that gives each node to each rule
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "rules.h"

/** Control flow levels that a function body can have */
#define MAX_NESTING_DEPTH 2

/** Bytes of source text that a parse-error message quotes, so that lines stay short */
#define QUOTE_LENGTH 40

/** Start of a Doxygen block comment */
#define DOC_COMMENT_OPEN "/**"

/**
 * End of a block comment. tree-sitter reads the string above as a comment start,
 * and this comment ends it.
 */
#define COMMENT_CLOSE "*/"

/** Doxygen command that documents a file */
#define FILE_COMMAND "@file"

/** Characters that end the file name after FILE_COMMAND */
#define WHITESPACE " \t\r\n"

/** Bit of a construct kind in a construct_t::may_contain mask */
#define BIT(kind) (1U << (kind))

/** Control flow constructs that the nesting rule knows, as indexes of constructs */
typedef enum {
    KIND_FUNCTION, /**< Function definition */
    KIND_IF,       /**< `if` statement and its `else if` chain */
    KIND_FOR,      /**< `for` loop */
    KIND_WHILE,    /**< `while` loop */
    KIND_DO,       /**< `do` loop */
    KIND_SWITCH,   /**< `switch` statement */
    KIND_CASE,     /**< `case` or `default` label of a switch */
    KIND_TERNARY,  /**< `?:` expression */
    KIND_GOTO,     /**< `goto` statement */
    KIND_COUNT,    /**< Number of kinds */
} construct_kind_t;

/** Mask of a construct that can contain no control flow */
#define NOTHING 0U

/** Mask of a switch, which contains only its cases */
#define CASES BIT(KIND_CASE)

/** Mask of a loop body, which can contain no while, do or switch */
#define LOOP_BODY (BIT(KIND_IF) | BIT(KIND_FOR) | BIT(KIND_TERNARY) | BIT(KIND_GOTO))

/** Mask of a function body, which can contain all constructs except a case */
#define FUNCTION_BODY (LOOP_BODY | BIT(KIND_WHILE) | BIT(KIND_DO) | BIT(KIND_SWITCH))

/** Control flow construct, with the constructs that it can contain */
typedef struct {
    const char *node_type;       /**< tree-sitter node type */
    const char *label;           /**< Name that the messages show */
    const char *continues_under; /**< Parent type of an `else if` chain, or NULL */
    unsigned    may_contain;     /**< BIT() mask of the constructs in its body */
    int         levels;          /**< Nesting levels that the construct adds */
} construct_t;

/** Constructs, indexed by construct_kind_t */
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

/** Innermost construct around a node, and the nesting depth there */
typedef struct {
    const construct_t *construct; /**< Innermost construct, or NULL outside functions */
    int                depth;     /**< Nesting levels around the node */
} scope_t;

/** Parent node of the nodes that the walk visits */
typedef struct {
    source_t   *source; /**< Source of the tree */
    const char *type;   /**< tree-sitter node type of the parent */
    scope_t     scope;  /**< Scope of the children */
} parent_t;

/** Node that the walk gives to each rule */
typedef struct {
    source_t          *source; /**< Source of the node */
    TSNode             node;   /**< Node that the rules check */
    const construct_t *opens;  /**< Construct that the node opens, or NULL */
    scope_t            scope;  /**< Scope around the node */
} visit_t;

/**
 * Checks one node and adds a finding for each violation.
 *
 * @param rule  Rule name for the findings.
 * @param visit Node and its context.
 */
typedef void rule_visitor_t(const char *rule, const visit_t *visit);

/** Rule that the walk runs on each node */
typedef struct {
    const char     *name;  /**< Rule name that the output shows */
    rule_visitor_t *visit; /**< Check of one node */
} rule_t;

/** Parts of the comment at the start of a file */
typedef struct {
    const char *body;     /**< Text after DOC_COMMENT_OPEN, or NULL */
    const char *body_end; /**< Start of COMMENT_CLOSE, or NULL */
    const char *command;  /**< FILE_COMMAND in the body, or NULL */
} docstring_t;

/** Check of a file docstring, with the message for a failure */
typedef struct {
    bool (*passes)(const docstring_t *docstring); /**< True if the docstring passes */
    const char *message;                          /**< Message for a failure */
} docstring_check_t;

/**
 * Finds the first occurrence of a string in a text that is not NUL-terminated.
 *
 * @param start  Start of the text.
 * @param end    End of the text.
 * @param needle String to find.
 * @return Start of the first occurrence, or NULL.
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
 * Tells if a text contains a letter or a digit.
 *
 * @param start Start of the text.
 * @param end   End of the text.
 * @return True if the text contains a letter or a digit.
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
 * Finds the parts of the comment at the start of a file.
 *
 * @param text   File text, without the wrapper.
 * @param length Bytes of the file text.
 * @return Parts of the comment. A part that is not there is NULL.
 */
static docstring_t find_docstring(const char *text, size_t length) {
    docstring_t docstring   = {.body = NULL, .body_end = NULL, .command = NULL};
    size_t      open_length = strlen(DOC_COMMENT_OPEN);
    // A slash right after DOC_COMMENT_OPEN makes an empty comment
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
 * Tells if the file starts with DOC_COMMENT_OPEN.
 *
 * @param docstring Parts of the file comment.
 * @return True if the comment has a body.
 */
static bool opens_doc_comment(const docstring_t *docstring) {
    return docstring->body != NULL;
}

/**
 * Tells if the file comment has an end.
 *
 * @param docstring Parts of the file comment.
 * @return True if the comment has COMMENT_CLOSE.
 */
static bool is_closed(const docstring_t *docstring) {
    return docstring->body_end != NULL;
}

/**
 * Tells if the file comment contains FILE_COMMAND.
 *
 * @param docstring Parts of the file comment.
 * @return True if the body contains FILE_COMMAND.
 */
static bool has_file_command(const docstring_t *docstring) {
    return docstring->command != NULL;
}

/**
 * Tells if the file comment has a description. The file name after FILE_COMMAND
 * is not part of the description.
 *
 * @param docstring Parts of the file comment.
 * @return True if a letter or a digit is in the body, outside the file name.
 */
static bool has_description(const docstring_t *docstring) {
    const char *name = docstring->command + strlen(FILE_COMMAND);
    name += strspn(name, BLANKS);
    if (has_alphanumeric(docstring->body, docstring->command)) {
        return true;
    }
    return has_alphanumeric(name + strcspn(name, WHITESPACE), docstring->body_end);
}

/** Each check needs the checks before it, so keep this order */
static const docstring_check_t docstring_checks[] = {
    {opens_doc_comment, "file does not begin with a /** comment"},
    {is_closed, "the opening /** comment is not closed"},
    {has_file_command, "the opening /** comment has no @file"},
    {has_description, "the @file comment has no description"},
};

/**
 * Checks that a file starts with a FILE_COMMAND comment that has a description.
 * Only the root node starts the check, so each file gets a maximum of one finding.
 *
 * @param rule  Rule name for the finding.
 * @param visit Node and its context.
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
 * Gives the BIT() of a construct from its position in constructs.
 *
 * @param construct Item of constructs.
 * @return Bit of the construct kind.
 */
static unsigned construct_bit(const construct_t *construct) {
    return BIT((unsigned)(construct - constructs));
}

/**
 * Checks that a construct is allowed in its scope and is not too deep.
 *
 * @param rule  Rule name for the finding.
 * @param visit Node and its context.
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
 * Gives the bytes of a text to quote: the rest of the line, to a maximum of
 * QUOTE_LENGTH.
 *
 * @param text Start of the quote.
 * @return Bytes to quote.
 */
static int quote_length(const char *text) {
    size_t length = strcspn(text, "\r\n");
    return (int)(length < QUOTE_LENGTH ? length : QUOTE_LENGTH);
}

/**
 * Adds a finding for each node that tree-sitter cannot parse or must add.
 *
 * @param rule  Rule name for the findings.
 * @param visit Node and its context.
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

/** Rules in the order of their findings at the same position */
static const rule_t rules[] = {
    {"file-docstring", check_file_docstring},
    {"nesting", check_nesting},
    {"parse-error", check_parse_error},
};

/**
 * Finds the construct of a node type.
 *
 * @param node_type tree-sitter node type.
 * @return Item of constructs, or NULL for a node that is not control flow.
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
 * Tells if a construct continues the chain of its parent, as `else if` does.
 *
 * @param construct   Construct of the node.
 * @param parent_type tree-sitter node type of the parent.
 * @return True if the node adds no new nesting level.
 */
static bool continues_chain(const construct_t *construct, const char *parent_type) {
    if (construct->continues_under == NULL) {
        return false;
    }
    return strcmp(construct->continues_under, parent_type) == 0;
}

/**
 * Finds the construct that a node opens. Outside a function, only a function
 * opens a construct.
 *
 * @param node   Node to examine.
 * @param parent Parent of the node.
 * @return Construct that the node opens, or NULL.
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
 * Gives the scope inside a construct.
 *
 * @param scope     Scope around the construct.
 * @param construct Construct that a node opens, or NULL.
 * @return Scope of the children of the node.
 */
static scope_t enter(scope_t scope, const construct_t *construct) {
    if (construct == NULL) {
        return scope;
    }
    return (scope_t){.construct = construct, .depth = scope.depth + construct->levels};
}

/**
 * Gives a node and all its descendants to each rule. On return, the cursor is on
 * the node again.
 *
 * @param cursor Cursor on the node.
 * @param parent Parent of the node.
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
