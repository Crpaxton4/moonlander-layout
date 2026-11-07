# Firmware, host programs, unit tests and static analysis of the programs in src/.
# CMakePresets.json gives the build directories and the settings, so CI, the hooks
# and editors that read presets use the same builds
cmake_minimum_required(VERSION 3.28)
project(moonlander_layout LANGUAGES C)

# ----------------------------------------------------------------------------------
# Shared settings
# ----------------------------------------------------------------------------------

# Each part needs only its own tools, so a job or a hook with fewer tools can select
# fewer parts
set(MOONLANDER_KNOWN_PARTS firmware checker converter unit-tests)
set(MOONLANDER_PARTS "${MOONLANDER_KNOWN_PARTS}" CACHE STRING
    "Parts to build: firmware, checker, converter, unit-tests")
foreach(part IN LISTS MOONLANDER_PARTS)
    if(NOT part IN_LIST MOONLANDER_KNOWN_PARTS)
        message(FATAL_ERROR "Unknown part '${part}' in MOONLANDER_PARTS")
    endif()
endforeach()

# Each host program and each unit test gets these flags, so all C code of the
# repository has the same strict rules. Every warning blocks the build
set(STRICT_C_FLAGS
    -std=c11
    -Werror
    # Usual warning sets, and ISO C only
    -Wall
    -Wextra
    -Wpedantic
    # Changes of value or sign in implicit conversions
    -Wconversion
    -Wsign-conversion
    # Declarations
    -Wshadow
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wmissing-declarations
    -Wold-style-definition
    -Wredundant-decls
    # Casts and pointers
    -Wcast-qual
    -Wcast-align
    -Wbad-function-cast
    -Wpointer-arith
    -Wwrite-strings
    # Preprocessor, floating point and formats
    -Wundef
    -Wdouble-promotion
    -Wfloat-equal
    -Wformat=2
    # Control flow and memory
    -Wswitch-enum
    -Wvla
    -Walloca
    -Wnull-dereference
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wjump-misses-init
    # GCC path-based static analysis
    -fanalyzer)
add_compile_options(${STRICT_C_FLAGS})
enable_testing()

# Each check target of a part adds itself to analyze. A check reads only the sources
# and a compile database, so analyze builds no program. Each check finds its tool on
# PATH when it runs, so the configure step needs no analysis tool
add_custom_target(analyze)

# Doxyfile reads the sources of all parts. Doxygen makes only the last directory of
# OUTPUT_DIRECTORY
add_custom_target(doxygen
    COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/.build
    COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=doxygen "-DCHECK_COMMAND=doxygen;Doxyfile"
        -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    VERBATIM)
add_dependencies(analyze doxygen)

# ----------------------------------------------------------------------------------
# Firmware: firmware, compile-databases, clang-tidy-keymap and gcc-strict
# ----------------------------------------------------------------------------------

# QMK builds keymaps only with its own make build, so the firmware targets start the
# QMK CLI. Each target finds qmk on PATH when it runs, so the configure step needs no
# QMK tool. The firmware targets are not in the all target, so a build of the host
# programs needs no QMK toolchain
if("firmware" IN_LIST MOONLANDER_PARTS)
    # QMK runs one job if nothing sets the number of jobs
    cmake_host_system_information(RESULT processor_count
        QUERY NUMBER_OF_LOGICAL_CORES)
    set(QMK_JOBS ${processor_count} CACHE STRING "Number of jobs of the QMK build")

    set(BUILD_FIRMWARE ${CMAKE_COMMAND} -DREPOSITORY=${PROJECT_SOURCE_DIR})
    add_custom_target(firmware
        COMMAND ${BUILD_FIRMWARE} "-DQMK_ARGUMENTS=userspace-compile;-j;${QMK_JOBS}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/build-firmware.cmake
        VERBATIM)

    # QMK writes its compile database into src/qmk/ and has no option for the
    # location. The configure step writes the compile databases of the host programs
    # and the unit tests, so this target writes only the keymap database
    set(KEYMAP_DATABASE ${PROJECT_SOURCE_DIR}/.build/compile_commands.json)
    set(QMK_DATABASE ${PROJECT_SOURCE_DIR}/src/qmk/compile_commands.json)
    set(qmk_compile_arguments compile -j ${QMK_JOBS} --compiledb
        -kb zsa/moonlander/reva -km qgmlwy)
    set(keymap_directory
        ${PROJECT_SOURCE_DIR}/src/qmk/keyboards/zsa/moonlander/keymaps/qgmlwy)
    add_custom_target(compile-databases
        COMMAND ${BUILD_FIRMWARE} "-DQMK_ARGUMENTS=${qmk_compile_arguments}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/build-firmware.cmake
        COMMAND ${CMAKE_COMMAND} -DQMK_DATABASE=${QMK_DATABASE}
            -DKEYMAP_C=${keymap_directory}/keymap.c
            -DCOMPILE_DATABASE=${KEYMAP_DATABASE}
            -P ${PROJECT_SOURCE_DIR}/src/ci/keymap-compile-database.cmake
        VERBATIM)

    # Each firmware file name contains the commit, so clean cannot name the firmware
    # files. clean removes the keymap database
    set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        ${KEYMAP_DATABASE} ${QMK_DATABASE})

    # Each keymap check stops if the keymap database is missing
    set(REQUIRE_KEYMAP_DATABASE ${CMAKE_COMMAND} -DCOMPILE_DATABASE=${KEYMAP_DATABASE}
        -P ${PROJECT_SOURCE_DIR}/src/ci/require-keymap-database.cmake)
    file(GLOB_RECURSE KEYMAP_SOURCES CONFIGURE_DEPENDS
        ${PROJECT_SOURCE_DIR}/src/qmk/keyboards/*.c)
    list(SORT KEYMAP_SOURCES)

    set(clang_tidy_keymap_command clang-tidy --quiet -p ${PROJECT_SOURCE_DIR}/.build
        ${KEYMAP_SOURCES})
    add_custom_target(clang-tidy-keymap
        COMMAND ${REQUIRE_KEYMAP_DATABASE}
        COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-keymap
            "-DCHECK_COMMAND=${clang_tidy_keymap_command}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
        VERBATIM)

    set(gcc_strict_command ${CMAKE_COMMAND} -DCOMPILE_DATABASE=${KEYMAP_DATABASE}
        -DUSERSPACE=${PROJECT_SOURCE_DIR}/src/qmk
        -P ${PROJECT_SOURCE_DIR}/src/ci/gcc-strict.cmake)
    add_custom_target(gcc-strict
        COMMAND ${REQUIRE_KEYMAP_DATABASE}
        COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=gcc-strict
            "-DCHECK_COMMAND=${gcc_strict_command}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
        VERBATIM)
    add_dependencies(analyze clang-tidy-keymap gcc-strict)
endif()

# ----------------------------------------------------------------------------------
# Standards checker
# ----------------------------------------------------------------------------------

if("checker" IN_LIST MOONLANDER_PARTS)
    # Checker needs the tree-sitter package versions that versions.env pins.
    # CMAKE_PREFIX_PATH can point to a `dpkg -x` extraction of the packages
    find_path(TREE_SITTER_INCLUDE_DIR tree_sitter/api.h REQUIRED)
    find_library(TREE_SITTER_LIBRARY libtree-sitter.a REQUIRED)

    # tree-sitter-c-src puts its parser in a directory with its version as name,
    # below the same prefix as the headers
    cmake_path(GET TREE_SITTER_INCLUDE_DIR PARENT_PATH tree_sitter_prefix)
    file(GLOB grammar_directories
        ${tree_sitter_prefix}/src/tree-sitter/c/*/parser/src)
    find_file(TREE_SITTER_C_GRAMMAR parser.c PATHS ${grammar_directories}
        NO_DEFAULT_PATH REQUIRED)

    # Generated code, so the build ignores its warnings and does not analyze it
    add_library(tree_sitter_c_grammar OBJECT ${TREE_SITTER_C_GRAMMAR})
    set_property(TARGET tree_sitter_c_grammar PROPERTY COMPILE_OPTIONS -w)
    target_include_directories(tree_sitter_c_grammar PRIVATE ${TREE_SITTER_INCLUDE_DIR})

    set(CHECKER_DIR ${PROJECT_SOURCE_DIR}/src/check_standards)
    set(CHECKER_SOURCES
        ${CHECKER_DIR}/main.c
        ${CHECKER_DIR}/rules.c
        ${CHECKER_DIR}/source.c
        ${CHECKER_DIR}/support.c)

    # Hook runs the program from the top of the build directory
    add_executable(check_standards ${CHECKER_SOURCES})
    set_property(TARGET check_standards PROPERTY RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR})
    target_include_directories(check_standards PRIVATE ${TREE_SITTER_INCLUDE_DIR})
    target_link_libraries(check_standards PRIVATE tree_sitter_c_grammar
        ${TREE_SITTER_LIBRARY})

    # CMake writes the compile database at the configure step, so clang-tidy does not
    # need a build of the checker
    set(clang_tidy_checker_command clang-tidy --quiet -p ${CMAKE_BINARY_DIR}
        ${CHECKER_SOURCES})
    add_custom_target(clang-tidy-checker
        COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-checker
            "-DCHECK_COMMAND=${clang_tidy_checker_command}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
        VERBATIM)
    add_dependencies(analyze clang-tidy-checker)

    # Each case directory holds <case>.expected, the expected checker output with the
    # exit status. Arguments are the .c, .h and .inc files of the case in sort order.
    # Names end in _first, _second or _third to set an order. Paths in the output
    # start at the repository root, so the case runs there
    set(CHECKER_TESTS_DIR ${PROJECT_SOURCE_DIR}/test/check_standards)
    file(GLOB checker_expectations CONFIGURE_DEPENDS ${CHECKER_TESTS_DIR}/*/*.expected)
    foreach(expected_file IN LISTS checker_expectations)
        cmake_path(GET expected_file STEM case)
        set(case_directory ${CHECKER_TESTS_DIR}/${case})
        file(GLOB case_files CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}
            ${case_directory}/*.c ${case_directory}/*.h ${case_directory}/*.inc)
        list(SORT case_files)
        add_test(NAME check_standards.${case}
            COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:check_standards>
                "-DPROGRAM_ARGUMENTS=${case_files}"
                -DOUTPUT_DIRECTORY=${CMAKE_BINARY_DIR}/test/check_standards/${case}
                -DEXPECTED_FILES=${expected_file} -DACTUAL_FILES=stdout
                -P ${PROJECT_SOURCE_DIR}/src/ci/run-fixture.cmake
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
        set_tests_properties(check_standards.${case} PROPERTIES LABELS checker)
    endforeach()
endif()

# ----------------------------------------------------------------------------------
# Key log converter
# ----------------------------------------------------------------------------------

if("converter" IN_LIST MOONLANDER_PARTS)
    set(CONVERTER_DIR ${PROJECT_SOURCE_DIR}/src/key_log_converter)
    set(CONVERTER_SOURCES
        ${CONVERTER_DIR}/converter.c
        ${CONVERTER_DIR}/counters.c
        ${CONVERTER_DIR}/hid_usage.c
        ${CONVERTER_DIR}/main.c
        ${CONVERTER_DIR}/markdown.c
        ${CONVERTER_DIR}/outputs.c
        ${CONVERTER_DIR}/profile.c
        ${CONVERTER_DIR}/record.c
        ${CONVERTER_DIR}/report.c
        ${CONVERTER_DIR}/support.c)

    # Program is at the top of the build directory, also in the Windows build
    add_executable(key_log_converter ${CONVERTER_SOURCES})
    set_property(TARGET key_log_converter PROPERTY RUNTIME_OUTPUT_DIRECTORY
        ${CMAKE_BINARY_DIR})

    # CMake writes the compile database at the configure step, so clang-tidy does not
    # need a build of the converter
    set(clang_tidy_converter_command clang-tidy --quiet -p ${CMAKE_BINARY_DIR}
        ${CONVERTER_SOURCES})
    add_custom_target(clang-tidy-converter
        COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-converter
            "-DCHECK_COMMAND=${clang_tidy_converter_command}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
        VERBATIM)
    add_dependencies(analyze clang-tidy-converter)

    # Each case directory holds its logs as <case>*.log in sort order and the expected
    # outputs as <case>.<output>, where <output> is a file name in the output folder of
    # the run. stdout and stderr are outputs too. Paths in the output start at the
    # repository root, so the case runs there
    set(CONVERTER_TESTS_DIR ${PROJECT_SOURCE_DIR}/test/key_log_converter)
    file(GLOB converter_stdouts CONFIGURE_DEPENDS ${CONVERTER_TESTS_DIR}/*/*.stdout)
    foreach(case_stdout IN LISTS converter_stdouts)
        cmake_path(GET case_stdout STEM case)
        set(case_directory ${CONVERTER_TESTS_DIR}/${case})
        file(GLOB case_logs CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}
            ${case_directory}/${case}*.log)
        list(SORT case_logs)

        file(GLOB expected_files CONFIGURE_DEPENDS ${case_directory}/${case}.*)
        list(FILTER expected_files EXCLUDE REGEX "\\.log$")
        set(actual_names "")
        string(LENGTH "${case}." prefix_length)
        foreach(expected_file IN LISTS expected_files)
            cmake_path(GET expected_file FILENAME expected_name)
            string(SUBSTRING "${expected_name}" ${prefix_length} -1 actual_name)
            list(APPEND actual_names ${actual_name})
        endforeach()

        set(output_directory ${CMAKE_BINARY_DIR}/test/key_log_converter/${case})
        add_test(NAME key_log_converter.${case}
            COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:key_log_converter>
                "-DPROGRAM_ARGUMENTS=${output_directory};${case_logs}"
                -DOUTPUT_DIRECTORY=${output_directory}
                "-DEXPECTED_FILES=${expected_files}" "-DACTUAL_FILES=${actual_names}"
                -P ${PROJECT_SOURCE_DIR}/src/ci/run-fixture.cmake
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
        set_tests_properties(key_log_converter.${case} PROPERTIES LABELS converter)
    endforeach()
endif()

# ----------------------------------------------------------------------------------
# Windows build
# ----------------------------------------------------------------------------------

# windows preset builds only the converter part, with the MinGW-w64 compiler.
# CMAKE_TOOLCHAIN_FILE must name a file, so the compiler settings stay in
# src/ci/mingw-w64-toolchain.cmake

# ----------------------------------------------------------------------------------
# Keymap unit tests
# ----------------------------------------------------------------------------------

# Keymap tests compile the keymap sources on the host against QMK's own keycode and
# type headers, so each keycode value is the value that the firmware uses
if("unit-tests" IN_LIST MOONLANDER_PARTS)
    find_package(cmocka 1.1.7 REQUIRED CONFIG)

    set(UNIT_TESTS_DIR ${PROJECT_SOURCE_DIR}/test/qmk)
    # Unit test programs, their libraries and their test runs use test/qmk/ in the
    # build directory, so the build directory mirrors test/
    set(UNIT_TESTS_BINARY_DIR ${CMAKE_BINARY_DIR}/test/qmk)

    # add_cmocka_test(<test source> [LIBRARIES <library>...])
    #
    # Builds one test program from the test source. Adds one CTest test for each
    # cmocka_unit_test*() case in the source, so a test runner shows each case on its
    # own. Each CTest test gives its case name as the first argument, and the test
    # program runs only the case with that name. A change to the test source runs the
    # configure step again, so a new case gets its CTest test at the next build
    function(add_cmocka_test test_source)
        cmake_parse_arguments(PARSE_ARGV 1 test "" "" LIBRARIES)
        get_filename_component(program ${test_source} NAME_WE)
        add_executable(${program} ${test_source})
        set_property(TARGET ${program} PROPERTY RUNTIME_OUTPUT_DIRECTORY
            ${UNIT_TESTS_BINARY_DIR})
        target_link_libraries(${program} PRIVATE ${test_LIBRARIES} cmocka::cmocka)
        set_property(DIRECTORY ${PROJECT_SOURCE_DIR} APPEND PROPERTY
            CMAKE_CONFIGURE_DEPENDS ${test_source})

        # clang-format can put the case name on the line after the macro name
        set(case_pattern "cmocka_unit_test[a-z_]*\\([ \t\n]*([a-z0-9_]+)")
        file(READ ${test_source} source_text)
        string(REGEX MATCHALL "${case_pattern}" case_entries "${source_text}")
        foreach(case_entry IN LISTS case_entries)
            string(REGEX REPLACE "${case_pattern}" "\\1" case "${case_entry}")
            add_test(NAME ${case} COMMAND ${program} ${case}
                WORKING_DIRECTORY ${UNIT_TESTS_BINARY_DIR})
            # cmocka passes when no case matches the name, so a run of no case fails.
            # `ctest -L qmk` selects the keymap unit tests with the label qmk
            set_tests_properties(${case} PROPERTIES
                FAIL_REGULAR_EXPRESSION "Running 0 test\\(s\\)"
                LABELS "${program};qmk")
        endforeach()
    endfunction()

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
    set(KEYMAP_DIR
        ${PROJECT_SOURCE_DIR}/src/qmk/keyboards/zsa/moonlander/keymaps/qgmlwy)

    # cmocka with the case selection
    add_library(unit_test STATIC ${UNIT_TESTS_DIR}/support/unit_test.c)
    target_include_directories(unit_test PUBLIC ${UNIT_TESTS_DIR}/support)
    target_link_libraries(unit_test PUBLIC cmocka::cmocka)

    # Recording QMK functions
    add_library(fake_qmk STATIC ${UNIT_TESTS_DIR}/support/fake_quantum.c)
    target_include_directories(fake_qmk PUBLIC ${UNIT_TESTS_DIR}/support ${KEYMAP_DIR})
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
        ${UNIT_TESTS_DIR}/support/keymap_introspection.c
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

    set_property(TARGET unit_test fake_qmk keymap_logic qgmlwy_keymap
        PROPERTY ARCHIVE_OUTPUT_DIRECTORY ${UNIT_TESTS_BINARY_DIR})

    set(KEYMAP_TESTS ${UNIT_TESTS_DIR}/keyboards/zsa/moonlander/keymaps/qgmlwy)
    add_cmocka_test(${KEYMAP_TESTS}/config_test.c LIBRARIES fake_qmk)
    add_cmocka_test(${KEYMAP_TESTS}/keymap_test.c LIBRARIES qgmlwy_keymap)
    add_cmocka_test(${KEYMAP_TESTS}/key_event_log/key_event_log_logic_test.c
        LIBRARIES keymap_logic unit_test)
    add_cmocka_test(${KEYMAP_TESTS}/case_modes/case_modes_logic_test.c
        LIBRARIES keymap_logic unit_test)
    add_cmocka_test(${KEYMAP_TESTS}/case_modes/case_modes_test.c
        LIBRARIES qgmlwy_keymap)
    add_cmocka_test(${KEYMAP_TESTS}/key_event_log/key_event_log_test.c
        LIBRARIES qgmlwy_keymap)
    add_cmocka_test(${KEYMAP_TESTS}/status_leds/status_leds_test.c
        LIBRARIES qgmlwy_keymap)
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
    add_cmocka_test(${KEYMAP_TESTS}/plain_mode/plain_mode_test.c
        LIBRARIES qgmlwy_keymap)

    # CMake writes the compile database at the configure step, so clang-tidy does not
    # need a build of the tests
    file(GLOB_RECURSE UNIT_TEST_SOURCES CONFIGURE_DEPENDS ${UNIT_TESTS_DIR}/*.c)
    list(SORT UNIT_TEST_SOURCES)
    set(clang_tidy_unit_tests_command clang-tidy --quiet -p ${CMAKE_BINARY_DIR}
        ${UNIT_TEST_SOURCES})
    add_custom_target(clang-tidy-unit-tests
        COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-unit-tests
            "-DCHECK_COMMAND=${clang_tidy_unit_tests_command}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
        VERBATIM)
    add_dependencies(analyze clang-tidy-unit-tests)
endif()
