/** @file
 * Case selection of the test programs.
 */

// #region Includes

#include "unit_test.h"

// #endregion

// #region Macros

/** Index of the case name in the arguments of main() */
#define CASE_NAME_ARGUMENT 1

// #endregion

// #region Public functions

void select_case(int argc, char **argv) {
    if (argc > CASE_NAME_ARGUMENT) {
        cmocka_set_test_filter(argv[CASE_NAME_ARGUMENT]);
    }
}

// #endregion
