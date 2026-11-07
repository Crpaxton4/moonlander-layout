/** @file
 * cmocka with the headers that it needs first, and the case selection that each
 * test program calls from main().
 */

#pragma once

#include <setjmp.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <cmocka.h>

/**
 * Makes cmocka run only the case that the first argument names.
 *
 * CTest gives one case name to each run, so each case is one CTest test. With no
 * argument, the program runs all its cases.
 *
 * @param argc Argument count of main().
 * @param argv Arguments of main().
 */
void select_case(int argc, char **argv);
