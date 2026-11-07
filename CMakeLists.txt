# Host programs and unit tests of the programs in src/. CMakePresets.json gives the
# build directories and the settings, so CI, the hooks and editors that read presets
# use the same builds
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

# make still builds the firmware, so the firmware part adds no target
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
