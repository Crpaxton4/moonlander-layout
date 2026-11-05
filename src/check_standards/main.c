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
 * rules.c holds the rules and the walk, source.c reads each file into the text that
 * tree-sitter parses, and support.c holds the code they share.
 */

#include <stdio.h>
#include <stdlib.h>
#include <tree_sitter/api.h>

#include "rules.h"

/** Index in `argv` of the first file to check. */
#define FIRST_FILE_ARGUMENT 1

/**
 * The C grammar, from the tree-sitter-c package's generated parser.
 *
 * @return The grammar, for ts_parser_set_language().
 */
const TSLanguage *tree_sitter_c(void);

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
