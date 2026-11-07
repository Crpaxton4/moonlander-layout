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

/** Time in ms between cursor movement reports while a mouse key is held */
#undef MOUSEKEY_INTERVAL
#define MOUSEKEY_INTERVAL 10

/** Time in ms between the first wheel step and the start of wheel repeat */
#undef MOUSEKEY_WHEEL_DELAY
#define MOUSEKEY_WHEEL_DELAY 200

/** Time in ms between the first cursor step and the start of cursor repeat */
#undef MOUSEKEY_DELAY
#define MOUSEKEY_DELAY 0

/** Top cursor speed, in steps of `MOUSEKEY_MOVE_DELTA` pixels per movement report */
#undef MOUSEKEY_MAX_SPEED
#define MOUSEKEY_MAX_SPEED 3

/** Number of movement reports the cursor takes to accelerate to its top speed */
#undef MOUSEKEY_TIME_TO_MAX
#define MOUSEKEY_TIME_TO_MAX 80

/** Top wheel speed, in steps of `MOUSEKEY_WHEEL_DELTA` wheel units per wheel report */
#undef MOUSEKEY_WHEEL_MAX_SPEED
#define MOUSEKEY_WHEEL_MAX_SPEED 6

/** Number of wheel reports the wheel takes to accelerate to its top speed */
#undef MOUSEKEY_WHEEL_TIME_TO_MAX
#define MOUSEKEY_WHEEL_TIME_TO_MAX 1

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
