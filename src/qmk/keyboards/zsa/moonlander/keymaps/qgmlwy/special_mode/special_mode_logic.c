/** @file
 * Double whitespace rule of the special mode.
 */

#include "special_mode/special_mode_logic.h"

void special_mode_start(special_mode_t *mode) {
    mode->last_push = SPECIAL_MODE_OTHER_KEY;
}

bool special_mode_push_ends_mode(special_mode_t *mode, enum special_mode_key key) {
    enum special_mode_key last_push = mode->last_push;
    mode->last_push                 = key;
    if (key == SPECIAL_MODE_OTHER_KEY) {
        return false;
    }
    return key == last_push;
}
