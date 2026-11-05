/** @file
 * Code tree-sitter can't parse: a GNU case range and a missing semicolon. The
 * nesting after the errors is still checked.
 */

int classify(int value) {
    switch (value) {
        case 1 ... 9:
            return 1;
        default:
            return 0;
    }
}

int broken(int value) {
    value = value + 1 return value;
}

int still_checked(int value) {
    if (value > 0) {
        if (value > 1) {
            return 2;
        }
    }
    return value;
}
