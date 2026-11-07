/** @file
 * Case selection of the test programs.
 */

#include "unit_test.h"

/** Index of the case name in the arguments of main() */
#define CASE_NAME_ARGUMENT 1

void select_case(int argc, char **argv) {
    if (argc > CASE_NAME_ARGUMENT) {
        cmocka_set_test_filter(argv[CASE_NAME_ARGUMENT]);
    }
}
