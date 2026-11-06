/** @file
 * The standards checker checks the C rules that no established tool checks.
 */

#include <stdio.h>
#include <stdlib.h>
#include <tree_sitter/api.h>

#include "rules.h"

#define FIRST_FILE_ARGUMENT 1

/* The tree-sitter-c package installs no header that declares this function. */
const TSLanguage *tree_sitter_c(void);

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
