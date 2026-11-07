# Builds the firmware with the QMK CLI and moves the firmware file into .build/. The
# file name ends in the commit, so each firmware file in .build/ identifies its source
#
#   cmake -DREPOSITORY=<folder> "-DQMK_ARGUMENTS=<argument>;..." -P build-firmware.cmake
#
# FIRMWARE_SUFFIX in the environment replaces the commit in the name. CI sets it for
# the PR and release builds
cmake_minimum_required(VERSION 3.28)
foreach(variable IN ITEMS REPOSITORY QMK_ARGUMENTS)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

set(FIRMWARE zsa_moonlander_reva_qgmlwy)
set(OUTPUT_DIRECTORY ${REPOSITORY}/.build)

# From the repository root, only QMK_USERSPACE points QMK to this src/qmk/. The make
# build of QMK must not get the settings of the build tool that runs this script
set(ENV{QMK_USERSPACE} ${REPOSITORY}/src/qmk)
unset(ENV{MAKEFLAGS})
unset(ENV{MFLAGS})
unset(ENV{MAKELEVEL})
find_program(QMK_PROGRAM qmk REQUIRED)
execute_process(COMMAND ${QMK_PROGRAM} ${QMK_ARGUMENTS}
    WORKING_DIRECTORY ${REPOSITORY}
    COMMAND_ERROR_IS_FATAL ANY)

# QMK copies the firmware into src/qmk/, but all build output goes into .build/
file(GLOB qmk_firmware_files ${REPOSITORY}/src/qmk/*.bin)
if(qmk_firmware_files STREQUAL "")
    message(FATAL_ERROR "QMK wrote no firmware file into ${REPOSITORY}/src/qmk")
endif()
file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY})
foreach(qmk_firmware_file IN LISTS qmk_firmware_files)
    cmake_path(GET qmk_firmware_file FILENAME file_name)
    file(RENAME ${qmk_firmware_file} ${OUTPUT_DIRECTORY}/${file_name})
endforeach()

# Outside a git work tree, the suffix is empty
if(DEFINED ENV{FIRMWARE_SUFFIX})
    set(suffix "$ENV{FIRMWARE_SUFFIX}")
else()
    execute_process(COMMAND git rev-parse --verify --quiet --short=7 HEAD
        WORKING_DIRECTORY ${REPOSITORY}
        OUTPUT_VARIABLE commit OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
    string(REGEX REPLACE "^(.+)$" "-\\1" suffix "${commit}")
endif()
set(name ${FIRMWARE}${suffix})

# With changes in src/qmk/ that have no commit, the commit does not identify the
# firmware, so the name also gets the start of its sha256
execute_process(COMMAND git status --porcelain -- src/qmk
    WORKING_DIRECTORY ${REPOSITORY}
    OUTPUT_VARIABLE changes ERROR_QUIET)
if(NOT changes STREQUAL "")
    file(SHA256 ${OUTPUT_DIRECTORY}/${FIRMWARE}.bin hash)
    string(SUBSTRING ${hash} 0 8 hash_start)
    string(APPEND name -dirty-${hash_start})
endif()

if(NOT name STREQUAL FIRMWARE)
    file(RENAME ${OUTPUT_DIRECTORY}/${FIRMWARE}.bin ${OUTPUT_DIRECTORY}/${name}.bin)
endif()
message("firmware .build/${name}.bin")
