# Host programs, unit tests and static analysis of the programs in src/.
# CMakePresets.json gives the build directories and the settings, so CI, the hooks
# and editors that read presets use the same builds
cmake_minimum_required(VERSION 3.28)
project(moonlander_layout LANGUAGES C)

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

include(src/ci/strict-c-flags.cmake)
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

# make still builds the firmware and its compile database, so the firmware part has
# only the keymap checks. Each one stops if that database is missing
if("firmware" IN_LIST MOONLANDER_PARTS)
    set(KEYMAP_DATABASE ${PROJECT_SOURCE_DIR}/.build/compile_commands.json)
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

if("checker" IN_LIST MOONLANDER_PARTS)
    add_subdirectory(src/check_standards)
    add_subdirectory(test/check_standards)
endif()
if("converter" IN_LIST MOONLANDER_PARTS)
    add_subdirectory(src/key_log_converter)
    add_subdirectory(test/key_log_converter)
endif()
if("unit-tests" IN_LIST MOONLANDER_PARTS)
    add_subdirectory(test/qmk)
endif()
