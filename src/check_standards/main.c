/** @file
 * The standards checker. It checks the C source rules of this repository that no
 * established tool checks.
 *
 * Usage: `check_standards FILE...`
 *
 * The checker gives each node of the tree-sitter syntax tree of each file to each
 * rule in `rules`. After the last file, it prints the findings, one on each line as
 * `file:line:column rule message`. The sort keys are the argument order of the
 * file, the line and the column. The last line is `N findings in M files`. The exit
 * status is 1 if there are findings, and 0 if there are none.
 *
 * rules.c contains the rules and the tree walk. source.c reads the files. support.c
 * contains the shared code.
 */

#include <stdio.h>
#include <stdlib.h>
#include <tree_sitter/api.h>

#include "rules.h"

/** The index in `argv` of the first file path. */
#define FIRST_FILE_ARGUMENT 1

/**
 * The C grammar. The generated parser of the tree-sitter-c package defines this
 * function.
 *
 * @return The grammar for ts_parser_set_language().
 */
const TSLanguage *tree_sitter_c(void);

/**
 * Checks one file and sorts its findings.
 *
 * @param parser   The parser.
 * @param findings The list that gets the findings.
 * @param path     The path of the file.
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
 * Checks each file on the command line, then prints the findings.
 *
 * @param argc The number of arguments, with the program name.
 * @param argv The program name, then the paths of the files.
 * @return EXIT_FAILURE if there are findings, or EXIT_SUCCESS if there are none.
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
