# Compiles each userspace file of the keymap compile database again with the QMK gcc
# and more warnings. Each warning fails the check
#
#   cmake -DCOMPILE_DATABASE=<file> -DUSERSPACE=<folder> -P gcc-strict.cmake
cmake_minimum_required(VERSION 3.28)
foreach(variable IN ITEMS COMPILE_DATABASE USERSPACE)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

# QMK's build cannot get these flags, because keymap.c compiles as a part of a
# QMK file
set(GCC_STRICT_FLAGS
    -Wextra
    -Wconversion
    -Wsign-conversion
    -Wshadow
    -Wmissing-prototypes
    -Wmissing-declarations
    -Wcast-qual
    -Wcast-align
    -Wpointer-arith
    -Wwrite-strings
    -Wundef
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wnull-dereference
    -Wjump-misses-init
    -Wdouble-promotion
    -Wformat=2
    -Wswitch-enum
    -Woverride-init
    -Wredundant-decls
    -Wold-style-definition
    -Wbad-function-cast
    -Wfloat-equal
    -Wvla
    -Walloca
    -fanalyzer)

# QMK adds the include directories and the predefined macros of gcc for clang
# tools, and gcc stops on a new definition of __STDC__. -o and the dependency
# flags write files into the QMK build directory. -fstack-usage writes a file into
# the working directory, which is the qmk_firmware checkout
set(FLAGS_WITH_VALUE -o -MF -MT -MQ -isystem)
set(FLAGS_WITHOUT_VALUE -MMD -MP -MD -fstack-usage)

# strict_arguments(<variable> <entry>)
#
# Sets <variable> to the gcc command of one database entry, with the flags above
# removed and the strict flags added
function(strict_arguments variable entry)
    string(JSON argument_count LENGTH "${entry}" arguments)
    math(EXPR last_index "${argument_count} - 1")
    set(arguments "")
    set(skip_value FALSE)
    foreach(index RANGE ${last_index})
        string(JSON argument GET "${entry}" arguments ${index})
        string(REGEX REPLACE "^-I" "" include_directory "${argument}")
        cmake_path(IS_PREFIX USERSPACE "${include_directory}" in_userspace)
        if(skip_value)
            set(skip_value FALSE)
        elseif(argument IN_LIST FLAGS_WITH_VALUE)
            set(skip_value TRUE)
        elseif(argument MATCHES "^-I" AND NOT in_userspace)
            # gcc gives no findings in system headers, so QMK's headers become
            # system headers. Findings in the userspace headers stay
            list(APPEND arguments -isystem "${include_directory}")
        elseif(NOT argument IN_LIST FLAGS_WITHOUT_VALUE
                AND NOT argument MATCHES "^-D__")
            list(APPEND arguments "${argument}")
        endif()
    endforeach()
    # -fanalyzer and -Wnull-dereference give no findings with -fsyntax-only, so
    # gcc compiles to /dev/null
    list(APPEND arguments ${GCC_STRICT_FLAGS} -Werror -o /dev/null)
    set(${variable} "${arguments}" PARENT_SCOPE)
endfunction()

file(READ "${COMPILE_DATABASE}" database)
string(JSON entry_count LENGTH "${database}")
math(EXPR last_entry "${entry_count} - 1")

# Only the userspace files get a compile, because QMK's own files do not compile
# without findings at these warning levels. Each file compiles also after a failed
# file, so each finding shows in one run
set(file_count 0)
set(failed_files "")
foreach(entry_index RANGE ${last_entry})
    string(JSON entry GET "${database}" ${entry_index})
    string(JSON source_file GET "${entry}" file)
    cmake_path(IS_PREFIX USERSPACE "${source_file}" in_userspace)
    if(NOT in_userspace)
        continue()
    endif()
    math(EXPR file_count "${file_count} + 1")
    string(JSON directory GET "${entry}" directory)
    strict_arguments(arguments "${entry}")
    execute_process(COMMAND ${arguments} WORKING_DIRECTORY "${directory}"
        RESULT_VARIABLE compile_result)
    if(NOT compile_result EQUAL 0)
        list(APPEND failed_files "${source_file}")
    endif()
endforeach()

if(file_count EQUAL 0)
    message(FATAL_ERROR "Found no entries in ${USERSPACE}")
endif()
list(LENGTH failed_files failed_count)
if(failed_count GREATER 0)
    list(JOIN failed_files "\n  " failed_list)
    message(FATAL_ERROR
        "${failed_count} of ${file_count} files have findings:\n  ${failed_list}")
endif()
