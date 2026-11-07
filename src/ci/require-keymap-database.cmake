# Stops a keymap check when the keymap compile database is missing. Without the
# database, the check reads the keymap with no QMK flags and its result is wrong
#
#   cmake -DCOMPILE_DATABASE=<file> -P require-keymap-database.cmake
if("${COMPILE_DATABASE}" STREQUAL "")
    message(FATAL_ERROR "COMPILE_DATABASE is not set or empty")
endif()
if(NOT EXISTS "${COMPILE_DATABASE}")
    message(FATAL_ERROR "Keymap compile database ${COMPILE_DATABASE} is missing. "
        "Run the compile-databases workflow first")
endif()
