# Keymap tests compile the keymap sources on the host against QMK's own keycode and
# type headers, so each keycode value is the value that the firmware uses

find_package(cmocka 1.1.7 REQUIRED CONFIG)

# add_cmocka_test(<test source> [LIBRARIES <library>...])
#
# Builds one test program from the test source and adds one CTest test for each
# cmocka_unit_test*() case in the source, so a test runner shows each case on its own.
# Each CTest test gives its case name as the first argument, and the test program
# runs only the case with that name. A change to the test source runs the configure
# step again, so a new case gets its CTest test at the next build
function(add_cmocka_test test_source)
    cmake_parse_arguments(PARSE_ARGV 1 test "" "" LIBRARIES)
    get_filename_component(program ${test_source} NAME_WE)
    add_executable(${program} ${test_source})
    target_link_libraries(${program} PRIVATE ${test_LIBRARIES} cmocka::cmocka)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${test_source})

    # clang-format can put the case name on the line after the macro name
    set(case_pattern "cmocka_unit_test[a-z_]*\\([ \t\n]*([a-z0-9_]+)")
    file(READ ${test_source} source_text)
    string(REGEX MATCHALL "${case_pattern}" case_entries "${source_text}")
    foreach(case_entry IN LISTS case_entries)
        string(REGEX REPLACE "${case_pattern}" "\\1" case "${case_entry}")
        add_test(NAME ${case} COMMAND ${program} ${case})
        # cmocka passes when no case matches the name, so a run of no case fails
        set_tests_properties(${case} PROPERTIES
            FAIL_REGULAR_EXPRESSION "Running 0 test\\(s\\)"
            LABELS ${program})
    endforeach()
endfunction()

# `ctest -L qmk` selects the keymap unit tests with this label
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
    ${KEYMAP_DIR}/special_mode/special_mode_logic.c
    ${KEYMAP_DIR}/case_modes/case_modes_logic.c
    ${KEYMAP_DIR}/bracket_pairs/bracket_pairs_logic.c)
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
    ${KEYMAP_DIR}/precision_key/precision_key.c
    ${KEYMAP_DIR}/case_modes/case_modes.c
    ${KEYMAP_DIR}/bracket_pairs/bracket_pairs.c
    ${KEYMAP_DIR}/plain_mode/plain_mode.c)
target_compile_definitions(qgmlwy_keymap PRIVATE KEYMAP_C="${KEYMAP_DIR}/keymap.c")
target_link_libraries(qgmlwy_keymap PUBLIC fake_qmk keymap_logic)

set(KEYMAP_TESTS keyboards/zsa/moonlander/keymaps/qgmlwy)
add_cmocka_test(${KEYMAP_TESTS}/config_test.c LIBRARIES fake_qmk)
add_cmocka_test(${KEYMAP_TESTS}/keymap_test.c LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/key_event_log/key_event_log_logic_test.c
    LIBRARIES keymap_logic unit_test)
add_cmocka_test(${KEYMAP_TESTS}/case_modes/case_modes_logic_test.c
    LIBRARIES keymap_logic unit_test)
add_cmocka_test(${KEYMAP_TESTS}/case_modes/case_modes_test.c LIBRARIES qgmlwy_keymap)
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
add_cmocka_test(${KEYMAP_TESTS}/bracket_pairs/bracket_pairs_logic_test.c
    LIBRARIES keymap_logic unit_test)
add_cmocka_test(${KEYMAP_TESTS}/bracket_pairs/bracket_pairs_test.c
    LIBRARIES qgmlwy_keymap)
add_cmocka_test(${KEYMAP_TESTS}/plain_mode/plain_mode_test.c LIBRARIES qgmlwy_keymap)

# CMake writes the compile database at the configure step, so clang-tidy does not
# need a build of the tests
file(GLOB_RECURSE UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.c)
list(SORT UNIT_TEST_SOURCES)
add_custom_target(clang-tidy-unit-tests
    COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-unit-tests
        "-DCHECK_COMMAND=clang-tidy;--quiet;-p;${CMAKE_BINARY_DIR};${UNIT_TEST_SOURCES}"
        -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
    VERBATIM)
add_dependencies(analyze clang-tidy-unit-tests)
