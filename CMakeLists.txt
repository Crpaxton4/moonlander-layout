# Unit tests of the programs in src/. CMakePresets.json gives the build directory and
# the settings, so `make qmk-test` and editors that read presets use the same build
cmake_minimum_required(VERSION 3.28)
project(moonlander_layout_tests LANGUAGES C)

find_package(cmocka 1.1.7 REQUIRED CONFIG)
enable_testing()

# Checker build reads the same file, so all C code of the repository gets the same
# strict flags. Lines that do not start with "-" are comments
file(STRINGS strict-c-flags.txt STRICT_C_FLAGS REGEX "^-")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS strict-c-flags.txt)
add_compile_options(${STRICT_C_FLAGS})

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

add_subdirectory(test/qmk)
