/** @file
 * Checker rules and the walk that gives each node to each rule
 */

#include <ctype.h>
#include <stdbool.h>
#include <string.h>

#include "rules.h"

#define MAX_NESTING_DEPTH 2

#define QUOTE_LENGTH 40

#define DOC_COMMENT_OPEN "/**"

/* tree-sitter reads the string above as a comment start. This comment ends it */
#define COMMENT_CLOSE "*/"

#define FILE_COMMAND "@file"

#define WHITESPACE " \t\r\n"

#define BIT(kind) (1U << (kind))

typedef enum {
    KIND_FUNCTION,
    KIND_IF,
    KIND_FOR,
    KIND_WHILE,
    KIND_DO,
    KIND_SWITCH,
    KIND_CASE,
    KIND_TERNARY,
    KIND_GOTO,
    KIND_COUNT,
} construct_kind_t;

#define NOTHING 0U

#define CASES BIT(KIND_CASE)

#define LOOP_BODY (BIT(KIND_IF) | BIT(KIND_FOR) | BIT(KIND_TERNARY) | BIT(KIND_GOTO))

#define FUNCTION_BODY (LOOP_BODY | BIT(KIND_WHILE) | BIT(KIND_DO) | BIT(KIND_SWITCH))

typedef struct {
    const char *node_type;
    const char *label;
    /** Under this parent type, the construct continues its chain, as `else if` does */
    const char *continues_under;
    unsigned    may_contain;
    int         levels;
} construct_t;

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

typedef struct {
    /** NULL outside function bodies */
    const construct_t *construct;
    int                depth;
} scope_t;

typedef struct {
    source_t   *source;
    const char *type;
    scope_t     scope;
} parent_t;

typedef struct {
    source_t          *source;
    TSNode             node;
    const construct_t *opens;
    scope_t            scope;
} visit_t;

typedef void rule_visitor_t(const char *rule, const visit_t *visit);

typedef struct {
    const char     *name;
    rule_visitor_t *visit;
} rule_t;

typedef struct {
    const char *body;
    const char *body_end;
    const char *command;
} docstring_t;

typedef struct {
    bool (*passes)(const docstring_t *docstring);
    const char *message;
} docstring_check_t;

static const char *find_text(const char *start, const char *end, const char *needle) {
    size_t length = strlen(needle);
    for (const char *at = start; (size_t)(end - at) >= length; at++) {
        if (memcmp(at, needle, length) == 0) {
            return at;
        }
    }
    return NULL;
}

static bool has_alphanumeric(const char *start, const char *end) {
    for (const char *at = start; at < end; at++) {
        if (isalnum((unsigned char)*at) != 0) {
            return true;
        }
    }
    return false;
}

static docstring_t find_docstring(const char *text, size_t length) {
    docstring_t docstring   = {.body = NULL, .body_end = NULL, .command = NULL};
    size_t      open_length = strlen(DOC_COMMENT_OPEN);
    /* A slash right after DOC_COMMENT_OPEN makes an empty comment */
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

static bool opens_doc_comment(const docstring_t *docstring) {
    return docstring->body != NULL;
}

static bool is_closed(const docstring_t *docstring) {
    return docstring->body_end != NULL;
}

static bool has_file_command(const docstring_t *docstring) {
    return docstring->command != NULL;
}

/** File name after FILE_COMMAND is not part of the description */
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

static unsigned construct_bit(const construct_t *construct) {
    return BIT((unsigned)(construct - constructs));
}

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

static int quote_length(const char *text) {
    size_t length = strcspn(text, "\r\n");
    return (int)(length < QUOTE_LENGTH ? length : QUOTE_LENGTH);
}

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

static const rule_t rules[] = {
    {"file-docstring", check_file_docstring},
    {"nesting", check_nesting},
    {"parse-error", check_parse_error},
};

static const construct_t *find_construct(const char *node_type) {
    for (size_t kind = 0; kind < KIND_COUNT; kind++) {
        if (strcmp(constructs[kind].node_type, node_type) == 0) {
            return &constructs[kind];
        }
    }
    return NULL;
}

static bool continues_chain(const construct_t *construct, const char *parent_type) {
    if (construct->continues_under == NULL) {
        return false;
    }
    return strcmp(construct->continues_under, parent_type) == 0;
}

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

static scope_t enter(scope_t scope, const construct_t *construct) {
    if (construct == NULL) {
        return scope;
    }
    return (scope_t){.construct = construct, .depth = scope.depth + construct->levels};
}

/** On return, the cursor is on the node again */
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
