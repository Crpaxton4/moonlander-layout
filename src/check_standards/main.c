/** @file
 * Standards checker entry point for the C rules that no established tool checks
 */

#include <stdio.h>
#include <stdlib.h>
#include <tree_sitter/api.h>

#include "rules.h"

/// argv index of the first file. argv[0] is the program name
#define FIRST_FILE_ARGUMENT 1

/**
 * Gives the C grammar. tree-sitter-c installs no header that declares this
 * function.
 *
 * @return C grammar of tree-sitter-c.
 */
const TSLanguage *tree_sitter_c(void);

/**
 * Checks one file and sorts its findings. The sort puts the findings of one file
 * in order and keeps the files in argument order.
 *
 * @param parser   Parser with the C grammar.
 * @param findings Findings of all files of the run.
 * @param path     File path from the command line.
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
 * Checks each file of the command line and prints the findings.
 *
 * @param argc Number of arguments.
 * @param argv Program name, then the file paths.
 * @return EXIT_SUCCESS with no findings, EXIT_FAILURE otherwise.
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
