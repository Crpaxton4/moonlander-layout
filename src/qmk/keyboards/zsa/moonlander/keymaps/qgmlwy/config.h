/** @file
 * Build-time settings for the qgmlwy keymap: USB wakeup, mouse keys and Oryx
 * identification.
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
 * Layout identifier that Oryx and Keymapp read to recognise this layout.
 * It is also the USB serial number string.
 */
#define SERIAL_NUMBER "LzzVe/pjxZ7X"
