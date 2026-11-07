/** @file
 * Build-time settings for the qgmlwy keymap: USB wakeup, mouse keys, status LEDs,
 * combos, tap-hold keys and Oryx identification.
 *
 * Each `#undef` clears a default that the keyboard or QMK may already have set (the
 * keyboard sets several mouse key values), so the value here wins without a
 * redefinition warning.
 */

/** Time in ms to wait after waking the host from USB suspend; 0 skips the wait */
#define USB_SUSPEND_WAKEUP_DELAY 0

/** Cursor counts of a tap, and the unit of each cursor speed */
#undef MOUSEKEY_MOVE_DELTA
#define MOUSEKEY_MOVE_DELTA 1

/**
 * Time in ms between the first cursor step and the start of cursor repeat, so that
 * a tap moves only one count. QMK keeps this value in steps of 10 ms.
 */
#undef MOUSEKEY_DELAY
#define MOUSEKEY_DELAY 80

/**
 * One cursor movement report each 10 ms while a mouse key is held. QMK sends the
 * next report only after more than this time in ms, so the value is one less.
 */
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 9

/**
 * Top cursor speed, in steps of `MOUSEKEY_MOVE_DELTA` counts per movement report.
 * The precision key sets QMK's copy of this value to one while it is held.
 */
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 20

/**
 * Number of movement reports the cursor takes to accelerate to its top speed. With
 * a top speed of 20, the speed increases by one count each four reports.
 */
#undef MOUSEKEY_TIME_TO_MAX
#define MOUSEKEY_TIME_TO_MAX 80

/**
 * Time in ms between the first wheel step and the start of wheel repeat, so that a
 * tap scrolls only one step. QMK keeps this value in steps of 10 ms.
 */
#undef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY 80

/**
 * One wheel step each 16 ms while a wheel key is held. QMK sends the next step only
 * after more than this time in ms, so the value is one less.
 */
#undef MOUSEKEY_WHEEL_INTERVAL
#define MOUSEKEY_WHEEL_INTERVAL 15

/** Top wheel speed, in steps of `MOUSEKEY_WHEEL_DELTA` wheel units per wheel step */
#undef MOUSEKEY_WHEEL_MAX_SPEED
#define MOUSEKEY_WHEEL_MAX_SPEED 8

/**
 * Number of wheel steps the wheel takes to accelerate to its top speed, about 1 s.
 * The speed increases by one unit each seven or eight steps.
 */
#undef MOUSEKEY_WHEEL_TIME_TO_MAX
#define MOUSEKEY_WHEEL_TIME_TO_MAX 60

/**
 * Mouse report tells the host that it can divide each wheel notch. Only the USB
 * descriptor uses this setting, so mouse keys need no pointing device driver.
 */
#define POINTING_DEVICE_HIRES_SCROLL_ENABLE

/**
 * Wheel units in one notch, when the host divides the notches. A tap then scrolls
 * 1/8 notch, and legacy programs add the units up to whole notches.
 */
#define POINTING_DEVICE_HIRES_SCROLL_MULTIPLIER 8

/**
 * Keymap sets all six status LEDs, so that only logging uses the logging LED. The
 * keyboard code then does not show the layer, and the LED_LEVEL key has no effect.
 */
#define MOONLANDER_USER_LEDS

/**
 * QMK finds the combo keys on this layer, LAYER_COMBO_POSITIONS, for each active
 * layer. Thus a combo is a set of key positions. process_combo.c cannot see the
 * layer names, so the value is a number. Positions that are not combo keys are
 * KC_TRANSPARENT on that layer: KC_NO is also the end marker of each combo key
 * list, so QMK would hold back each push of a KC_NO position as a combo key.
 */
#define COMBO_ONLY_FROM_LAYER 2

/** Time in ms that a tap-hold key waits for its release before it holds */
#define TAPPING_TERM 250

/** get_tapping_term() gives the term of each key, for the shorter Shift term */
#define TAPPING_TERM_PER_KEY

/** Tapping term in ms of the two Shift home row keys */
#define SHIFT_TAPPING_TERM 200

/**
 * A tap of another key during the tapping term holds the tap-hold key. The keymap
 * does not use HOLD_ON_OTHER_KEY_PRESS or RETRO_TAPPING, because with home row mods
 * they send modifiers in normal typing.
 */
#define PERMISSIVE_HOLD

/** Tap-hold key and a key of the same hand give two letters, not a modifier */
#define CHORDAL_HOLD

/** Time in ms after a letter in which a tap-hold key only taps */
#define FLOW_TAP_TERM 150

/** Tap and then hold of a tap-hold key holds it, and does not repeat the tap */
#define QUICK_TAP_TERM 0

/**
 * Layout identifier that Oryx and Keymapp read to identify the layout.
 * It is also the USB serial number string.
 */
#define SERIAL_NUMBER "LzzVe/pjxZ7X"
