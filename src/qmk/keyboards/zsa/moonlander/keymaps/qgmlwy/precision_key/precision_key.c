/** @file
 * Top cursor speed of the precision key.
 */

// #region Includes

#include QMK_KEYBOARD_H
#include "mousekey.h"

#include "precision_key/precision_key.h"

// #endregion

// #region Macros

/** Top cursor speed while the precision key is held, in counts per report */
#define PRECISION_TOP_SPEED 1

// #endregion

// #region Data

/** Top cursor speed before the push of the precision key */
static uint8_t normal_top_speed;

// #endregion

// #region Public functions

bool process_precision_key(uint16_t keycode, keyrecord_t *record) {
    if (keycode != QK_MOUSE_ACCELERATION_0) {
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
