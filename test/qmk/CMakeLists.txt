# Keymap tests compile the keymap sources on the host against QMK's own keycode and
# type headers, so each keycode value is the value that the firmware uses

# `make qmk-test` filters on this label
set_property(DIRECTORY PROPERTY LABELS qmk)

# QMK CLI knows the qmk_firmware checkout that the firmware build uses: the CLI
# setting user.qmk_home, the QMK_HOME environment variable, or its own default
set(QMK_HOME "$ENV{QMK_HOME}" CACHE PATH
    "ZSA qmk_firmware checkout; empty asks the QMK CLI")
if(QMK_HOME STREQUAL "")
    find_program(QMK_CLI qmk)
endif()
if(QMK_HOME STREQUAL "" AND QMK_CLI)
    execute_process(COMMAND ${QMK_CLI} env QMK_HOME
        OUTPUT_VARIABLE qmk_firmware OUTPUT_STRIP_TRAILING_WHITESPACE
        COMMAND_ERROR_IS_FATAL ANY)
else()
    set(qmk_firmware "${QMK_HOME}")
endif()
if(NOT EXISTS "${qmk_firmware}/quantum/keycodes.h")
    message(FATAL_ERROR "No ZSA qmk_firmware checkout at '${qmk_firmware}'. "
        "Run setup.sh, or set QMK_HOME to the checkout of src/qmk/.qmk-ref")
endif()
message(STATUS "QMK firmware: ${qmk_firmware}")

# QMK headers are system headers, so the strict flags show findings only in the
# keymap and in the tests
add_library(qmk_headers INTERFACE)
target_include_directories(qmk_headers SYSTEM INTERFACE
    ${qmk_firmware}/quantum
    ${qmk_firmware}/quantum/keymap_extras
    ${qmk_firmware}/quantum/process_keycode
    ${qmk_firmware}/quantum/sequencer
    ${qmk_firmware}/platforms
    ${qmk_firmware}/tmk_core/protocol)

# Keymap sources include their own headers by paths from this directory
set(KEYMAP_DIR ${PROJECT_SOURCE_DIR}/src/qmk/keyboards/zsa/moonlander/keymaps/qgmlwy)

# cmocka with the case selection
add_library(unit_test STATIC support/unit_test.c)
target_include_directories(unit_test PUBLIC support)
target_link_libraries(unit_test PUBLIC cmocka::cmocka)

# Recording QMK functions
add_library(fake_qmk STATIC support/fake_quantum.c)
target_include_directories(fake_qmk PUBLIC support ${KEYMAP_DIR})
target_compile_definitions(fake_qmk PUBLIC QMK_KEYBOARD_H="fake_keyboard.h"
    # Same feature as COMBO_ENABLE in rules.mk
    COMBO_ENABLE)
# QMK includes the keymap config.h before each source, so the tests get the same
# settings
target_compile_options(fake_qmk PUBLIC -include ${KEYMAP_DIR}/config.h)
target_link_libraries(fake_qmk PUBLIC qmk_headers unit_test)

# Logic of the keymap modules includes no QMK header, so its tests need no fake
add_library(keymap_logic STATIC
    ${KEYMAP_DIR}/key_event_log/key_event_log_logic.c
    ${KEYMAP_DIR}/special_mode/special_mode_logic.c)
target_include_directories(keymap_logic PUBLIC ${KEYMAP_DIR})

# Same keymap sources as rules.mk, with the logic sources in keymap_logic.
# keymap_introspection.c compiles keymap.c
add_library(qgmlwy_keymap STATIC
    support/keymap_introspection.c
    ${KEYMAP_DIR}/tap_hold/tap_hold.c
    ${KEYMAP_DIR}/key_event_log/key_event_log.c
    ${KEYMAP_DIR}/special_mode/special_mode.c
    ${KEYMAP_DIR}/status_leds/status_leds.c
    ${KEYMAP_DIR}/symbol_holds/symbol_holds.c
    ${KEYMAP_DIR}/precision_key/precision_key.c)
target_compile_definitions(qgmlwy_keymap PRIVATE KEYMAP_C="${KEYMAP_DIR}/keymap.c")
target_link_libraries(qgmlwy_keymap PUBLIC fake_qmk keymap_logic)

set(KEYMAP_TESTS keyboards/zsa/moonlander/keymaps/qgmlwy)
add_cmocka_test(${KEYMAP_TESTS}/config_test.c LIBRARIES fake_qmk)
add_cmocka_test(${KEYMAP_TESTS}/keymap_test.c LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/key_event_log/key_event_log_logic_test.c
    LIBRARIES keymap_logic unit_test)
add_cmocka_test(${KEYMAP_TESTS}/key_event_log/key_event_log_test.c
    LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/status_leds/status_leds_test.c LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/precision_key/precision_key_test.c
    LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/symbol_holds/symbol_holds_test.c
    LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/tap_hold/tap_hold_test.c LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/special_mode/special_mode_logic_test.c
    LIBRARIES keymap_logic unit_test)
add_cmocka_test(${KEYMAP_TESTS}/special_mode/special_mode_test.c
    LIBRARIES qgmlwy_keymap)
