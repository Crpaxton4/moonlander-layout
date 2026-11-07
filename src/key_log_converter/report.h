/** @file
 * Markdown summary of a run: usage, profiling and data quality
 */

#pragma once

#include <stdio.h>

#include "converter.h"

/**
 * Writes the summary. The number formats are fixed and the sort orders are stable,
 * so the same logs give the same bytes on all systems.
 *
 * @param file      Open output file.
 * @param converter Totals of the run. The tables of names get their sort order.
 */
void write_report(FILE *file, converter_t *converter);
