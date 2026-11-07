# Builds the firmware with the QMK CLI and copies the firmware file into .build/. The
# file name ends in the commit, so each firmware file in .build/ identifies its source
#
#   cmake -DREPOSITORY=<folder> "-DQMK_ARGUMENTS=<argument>;..."
#       -DQMK_BUILD_DIRECTORY=<folder> -DSTACK_USAGE_DIRECTORY=<folder>
#       -DNAME_TAG=<tag> -P build-firmware.cmake
#
# QMK writes its object files into QMK_BUILD_DIRECTORY. The stack usage files go into
# STACK_USAGE_DIRECTORY. A NAME_TAG that is not empty goes at the end of the file name.
# FIRMWARE_SUFFIX in the environment replaces the commit in the name. CI sets it for
# the PR and release builds
cmake_minimum_required(VERSION 3.28)
foreach(variable IN ITEMS REPOSITORY QMK_ARGUMENTS QMK_BUILD_DIRECTORY
        STACK_USAGE_DIRECTORY)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()
if(NOT DEFINED NAME_TAG)
    message(FATAL_ERROR "NAME_TAG is not set")
endif()

set(FIRMWARE zsa_moonlander_reva_qgmlwy)
set(OUTPUT_DIRECTORY ${REPOSITORY}/.build)

# Each build has its own QMK build directory, so builds with different settings or
# from different work trees never share object files. These flags change no byte of
# the firmware. The linker shows the use of each memory region, and gcc writes the
# stack use of each function next to the object file
set(QMK_BUILD_ARGUMENTS
    -e BUILD_DIR=${QMK_BUILD_DIRECTORY}
    -e EXTRAFLAGS=-fstack-usage
    -e EXTRALDFLAGS=-Wl,--print-memory-usage)

# QMK lists the compile commands with `make -n`, thus the compile database has only
# the files that compile again. QMK cleans only its own build directory first
if("--compiledb" IN_LIST QMK_ARGUMENTS)
    file(REMOVE_RECURSE ${QMK_BUILD_DIRECTORY})
endif()

# From the repository root, only QMK_USERSPACE points QMK to this src/qmk/. The make
# build of QMK must not get the settings of the build tool that runs this script
set(ENV{QMK_USERSPACE} ${REPOSITORY}/src/qmk)
unset(ENV{MAKEFLAGS})
unset(ENV{MFLAGS})
unset(ENV{MAKELEVEL})
find_program(QMK_PROGRAM qmk REQUIRED)
execute_process(COMMAND ${QMK_PROGRAM} ${QMK_ARGUMENTS} ${QMK_BUILD_ARGUMENTS}
    WORKING_DIRECTORY ${REPOSITORY}
    COMMAND_ERROR_IS_FATAL ANY)

# QMK also copies the firmware into src/qmk/, but all build output goes into .build/
file(REMOVE ${REPOSITORY}/src/qmk/${FIRMWARE}.bin)
set(qmk_firmware_file ${QMK_BUILD_DIRECTORY}/${FIRMWARE}.bin)
if(NOT EXISTS ${qmk_firmware_file})
    message(FATAL_ERROR "QMK wrote no firmware file ${qmk_firmware_file}")
endif()

# Files of an earlier build must not go into the memory report
file(REMOVE_RECURSE ${STACK_USAGE_DIRECTORY})
file(GLOB_RECURSE stack_usage_files RELATIVE ${QMK_BUILD_DIRECTORY}
    ${QMK_BUILD_DIRECTORY}/*.su)
if(stack_usage_files STREQUAL "")
    message(FATAL_ERROR "QMK wrote no stack usage file into ${QMK_BUILD_DIRECTORY}")
endif()
foreach(stack_usage_file IN LISTS stack_usage_files)
    cmake_path(GET stack_usage_file PARENT_PATH relative_directory)
    file(COPY ${QMK_BUILD_DIRECTORY}/${stack_usage_file}
        DESTINATION ${STACK_USAGE_DIRECTORY}/${relative_directory})
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
    file(SHA256 ${qmk_firmware_file} hash)
    string(SUBSTRING ${hash} 0 8 hash_start)
    string(APPEND name -dirty-${hash_start})
endif()

if(NOT NAME_TAG STREQUAL "")
    string(APPEND name -${NAME_TAG})
endif()

file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY})
file(COPY_FILE ${qmk_firmware_file} ${OUTPUT_DIRECTORY}/${name}.bin)
message("firmware .build/${name}.bin")
