CONSOLE_ENABLE = yes
COMMAND_ENABLE = no
AUDIO_ENABLE = no
ORYX_ENABLE = yes
RGB_MATRIX_CUSTOM_KB = yes
COMBO_ENABLE = yes
SPACE_CADET_ENABLE = no

SRC += tap_hold/tap_hold.c
SRC += key_event_log/key_event_log_logic.c
SRC += key_event_log/key_event_log.c
SRC += special_mode/special_mode_logic.c
SRC += special_mode/special_mode.c
SRC += status_leds/status_leds.c
SRC += symbol_holds/symbol_holds.c
SRC += precision_key/precision_key.c
SRC += case_modes/case_modes_logic.c
SRC += case_modes/case_modes.c
SRC += bracket_pairs/bracket_pairs_logic.c
SRC += bracket_pairs/bracket_pairs.c
SRC += plain_mode/plain_mode.c
SRC += combos/combos.c

# Profiling debug build: qmk compile -e DEBUG_PROFILE=yes. The normal firmware
# compiles no profiling source, so its bytes stay the same
ifeq ($(strip $(DEBUG_PROFILE)), yes)
OPT_DEFS += -DDEBUG_PROFILE
SRC += profiling/profiling_logic.c
SRC += profiling/profiling_hardware.c
SRC += profiling/profiling.c
endif
