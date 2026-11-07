set(CONVERTER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/converter.c
    ${CMAKE_CURRENT_SOURCE_DIR}/counters.c
    ${CMAKE_CURRENT_SOURCE_DIR}/hid_usage.c
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/outputs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/record.c
    ${CMAKE_CURRENT_SOURCE_DIR}/support.c)

# Program is at the top of the build directory, also in the Windows build
add_executable(key_log_converter ${CONVERTER_SOURCES})
set_property(TARGET key_log_converter PROPERTY RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR})

# CMake writes the compile database at the configure step, so clang-tidy does not
# need a build of the converter
add_custom_target(clang-tidy-converter
    COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-converter
        "-DCHECK_COMMAND=clang-tidy;--quiet;-p;${CMAKE_BINARY_DIR};${CONVERTER_SOURCES}"
        -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
    VERBATIM)
