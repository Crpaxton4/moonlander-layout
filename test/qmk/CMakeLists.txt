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
    ${qmk_firmware}/platforms)

# Keymap sources include their own headers by paths from this directory
set(KEYMAP_DIR ${PROJECT_SOURCE_DIR}/src/qmk/keyboards/zsa/moonlander/keymaps/qgmlwy)
