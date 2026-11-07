/** @file
 * One-shot and double whitespace rule of the special mode.
 */

// #region Includes

#include "special_mode/special_mode_logic.h"

// #endregion

// #region Public functions

void special_mode_start(special_mode_t *mode, enum special_mode_kind kind) {
    mode->kind      = kind;
    mode->last_push = SPECIAL_MODE_OTHER_KEY;
}

enum special_mode_push_result special_mode_push(special_mode_t       *mode,
                                                enum special_mode_key key) {
    enum special_mode_key last_push = mode->last_push;
    mode->last_push                 = key;
    if (mode->kind == SPECIAL_MODE_ONE_SHOT) {
        return SPECIAL_MODE_ENDS_AFTER_KEY;
    } else if (key == SPECIAL_MODE_OTHER_KEY || key != last_push) {
        return SPECIAL_MODE_STAYS_ON;
    }
    return SPECIAL_MODE_ENDS_WITH_NO_KEY;
}

// #endregion
