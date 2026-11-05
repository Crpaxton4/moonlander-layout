/** @file
 * Text tree-sitter can't parse in place but the rules don't need: a table in
 * PROGMEM, AVR's program memory, whose rows are included inside its initializer.
 */

#include <stdint.h>

static const uint8_t PROGMEM rows[][2] = {
#include "../docstring_clean/table_row.inc"
#include "../docstring_missing/table_row.inc"
};

uint8_t first_cell(void) {
    return rows[0][0];
}
