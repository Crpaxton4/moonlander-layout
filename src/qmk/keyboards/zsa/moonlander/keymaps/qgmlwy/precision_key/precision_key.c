/** @file
 * Top cursor speed of the precision hold.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "mousekey.h"

#include "keycodes/keycodes.h"
#include "precision_key/precision_key.h"

// #endregion

// #region Macros

/** Top cursor speed while the precision hold lasts, in counts per report */
#define PRECISION_TOP_SPEED 1

// #endregion

// #region Data

/** Top cursor speed before the start of the precision hold */
static uint8_t normal_top_speed;

// #endregion

// #region Public functions

bool process_precision_key(uint16_t keycode, keyrecord_t *record) {
    if (keycode != PRECISION_HOLD_KEY || record->tap.count > 0) {
        return true;
    }
    if (record->event.pressed) {
        normal_top_speed = mk_max_speed;
        mk_max_speed     = PRECISION_TOP_SPEED;
    } else {
        mk_max_speed = normal_top_speed;
    }
    return false;
}

// #endregion
