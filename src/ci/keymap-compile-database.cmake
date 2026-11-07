# Writes the keymap compile database from the QMK compile database, and removes the
# QMK compile database. QMK compiles keymap.c only as an include of
# quantum/keymap_introspection.c, so its database has no keymap.c entry. Editors then
# cannot resolve QMK_KEYBOARD_H in keymap.c. The new entry copies the introspection
# command, with the prelude that introspection includes before keymap.c
#
#   cmake -DQMK_DATABASE=<file> -DKEYMAP_C=<file> -DCOMPILE_DATABASE=<file>
#       -P keymap-compile-database.cmake
cmake_minimum_required(VERSION 3.28)
foreach(variable IN ITEMS QMK_DATABASE KEYMAP_C COMPILE_DATABASE)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

set(INTROSPECTION quantum/keymap_introspection.c)

# json_string(<variable> <text>)
#
# Sets <variable> to <text> as a JSON string. Compile arguments contain no control
# characters, so only backslashes and quotes need an escape
function(json_string variable text)
    string(REPLACE "\\" "\\\\" text "${text}")
    string(REPLACE "\"" "\\\"" text "${text}")
    set(${variable} "\"${text}\"" PARENT_SCOPE)
endfunction()

# json_append(<array variable> <text>)
#
# Appends <text> as a JSON string to the JSON array in <array variable>
function(json_append array_variable text)
    json_string(value "${text}")
    string(JSON length LENGTH "${${array_variable}}")
    string(JSON array SET "${${array_variable}}" ${length} "${value}")
    set(${array_variable} "${array}" PARENT_SCOPE)
endfunction()

file(READ "${QMK_DATABASE}" database)
string(JSON entry_count LENGTH "${database}")
math(EXPR last_entry "${entry_count} - 1")
set(introspection_count 0)
set(keymap_indexes "")
foreach(entry_index RANGE ${last_entry})
    string(JSON source_file GET "${database}" ${entry_index} file)
    if(source_file STREQUAL INTROSPECTION)
        string(JSON introspection_entry GET "${database}" ${entry_index})
        math(EXPR introspection_count "${introspection_count} + 1")
    elseif(source_file STREQUAL KEYMAP_C)
        list(PREPEND keymap_indexes ${entry_index})
    endif()
endforeach()
if(NOT introspection_count EQUAL 1)
    message(FATAL_ERROR
        "Expected one ${INTROSPECTION} entry, found ${introspection_count}")
endif()

# Removal starts at the last index, so each index that follows stays correct
foreach(entry_index IN LISTS keymap_indexes)
    string(JSON database REMOVE "${database}" ${entry_index})
endforeach()

string(JSON argument_count LENGTH "${introspection_entry}" arguments)
math(EXPR last_argument "${argument_count} - 1")
set(arguments "[]")
foreach(argument_index RANGE ${last_argument})
    string(JSON argument GET "${introspection_entry}" arguments ${argument_index})
    if(argument STREQUAL INTROSPECTION)
        json_append(arguments -include)
        json_append(arguments community_modules_introspection.h)
        json_append(arguments "${KEYMAP_C}")
    else()
        json_append(arguments "${argument}")
    endif()
endforeach()

json_string(keymap_value "${KEYMAP_C}")
string(JSON keymap_entry SET "${introspection_entry}" arguments "${arguments}")
string(JSON keymap_entry SET "${keymap_entry}" file "${keymap_value}")
string(JSON entry_count LENGTH "${database}")
string(JSON database SET "${database}" ${entry_count} "${keymap_entry}")
file(WRITE "${COMPILE_DATABASE}" "${database}\n")
file(REMOVE "${QMK_DATABASE}")
