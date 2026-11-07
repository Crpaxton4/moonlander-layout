CONSOLE_ENABLE = yes
COMMAND_ENABLE = no
AUDIO_ENABLE = no
ORYX_ENABLE = yes
RGB_MATRIX_CUSTOM_KB = yes
TAP_DANCE_ENABLE = yes
COMBO_ENABLE = yes
SPACE_CADET_ENABLE = no

SRC += tap_dance/tap_dance.c
SRC += tap_dance/dances.c
SRC += tap_hold/tap_hold.c
SRC += key_event_log/key_event_log_logic.c
SRC += key_event_log/key_event_log.c
SRC += special_mode/special_mode_logic.c
SRC += special_mode/special_mode.c
SRC += status_leds/status_leds.c
SRC += symbol_holds/symbol_holds.c
