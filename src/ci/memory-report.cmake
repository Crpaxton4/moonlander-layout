# Writes the memory report of the firmware builds into REPORT_FILE and shows it. For
# each build, the report gives the flash and RAM use, the largest symbols and the
# largest stack frames. The report reads the build output and builds nothing
#
#   cmake -DREPOSITORY=<folder> "-DBUILD_NAMES=<name>;..." -DQMK_BUILD_ROOT=<folder>
#       -DSTACK_USAGE_ROOT=<folder> -DREPORT_FILE=<file> -P memory-report.cmake
#
# Each build has its .elf file in QMK_BUILD_ROOT/<name>/ and its stack usage files
# in STACK_USAGE_ROOT/<name>/
cmake_minimum_required(VERSION 3.28)
foreach(variable IN ITEMS REPOSITORY BUILD_NAMES QMK_BUILD_ROOT STACK_USAGE_ROOT
        REPORT_FILE)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

set(LIST_LENGTH 20)
find_program(NM_PROGRAM arm-none-eabi-nm REQUIRED)

# pad_left(<variable> <width> <text>)
#
# Sets <variable> to <text> with spaces before it, to a length of <width>
function(pad_left variable width text)
    string(LENGTH "${text}" length)
    set(padding "")
    if(length LESS width)
        math(EXPR padding_length "${width} - ${length}")
        string(REPEAT " " ${padding_length} padding)
    endif()
    set(${variable} "${padding}${text}" PARENT_SCOPE)
endfunction()

# pad_right(<variable> <width> <text>)
#
# Sets <variable> to <text> with spaces after it, to a length of <width>
function(pad_right variable width text)
    string(LENGTH "${text}" length)
    set(padding "")
    if(length LESS width)
        math(EXPR padding_length "${width} - ${length}")
        string(REPEAT " " ${padding_length} padding)
    endif()
    set(${variable} "${text}${padding}" PARENT_SCOPE)
endfunction()

# percent(<variable> <part> <whole>)
#
# Sets <variable> to <part> as a percentage of <whole>, rounded to two decimals as
# the linker does
function(percent variable part whole)
    math(EXPR hundredths "(${part} * 10000 + ${whole} / 2) / ${whole}")
    math(EXPR whole_percent "${hundredths} / 100")
    math(EXPR fraction "${hundredths} % 100 + 100")
    string(SUBSTRING "${fraction}" 1 2 fraction)
    set(${variable} "${whole_percent}.${fraction} %" PARENT_SCOPE)
endfunction()

# symbol_value(<variable> <nm output> <symbol>)
#
# Sets <variable> to the value of <symbol> in the decimal nm output, without the
# zeros that nm puts before each value
function(symbol_value variable symbols symbol)
    string(REGEX MATCH "\n0*([0-9]+) [A-Za-z?] ${symbol}\n" match "\n${symbols}")
    if(match STREQUAL "")
        message(FATAL_ERROR "Firmware has no symbol ${symbol}")
    endif()
    set(${variable} ${CMAKE_MATCH_1} PARENT_SCOPE)
endfunction()

# memory_use(<variable> <elf file>)
#
# Sets <variable> to the lines of the flash and RAM use. The ChibiOS linker script
# gives all free RAM to the heap, thus the linker shows the RAM as full. The RAM line
# gives the RAM without the heap
function(memory_use variable elf_file)
    execute_process(COMMAND ${NM_PROGRAM} --radix=d ${elf_file}
        OUTPUT_VARIABLE symbols COMMAND_ERROR_IS_FATAL ANY)
    set(lines "")
    foreach(region IN ITEMS flash0 ram0)
        symbol_value(base "${symbols}" __${region}_base__)
        symbol_value(free "${symbols}" __${region}_free__)
        symbol_value(size "${symbols}" __${region}_size__)
        math(EXPR used "${free} - ${base}")
        percent(used_percent ${used} ${size})
        pad_right(region_text 6 ${region})
        pad_left(used_text 7 ${used})
        pad_left(size_text 7 ${size})
        string(APPEND lines "${region_text} ${used_text} of ${size_text} bytes, "
            "${used_percent}\n")
    endforeach()
    symbol_value(heap_base "${symbols}" __heap_base__)
    symbol_value(heap_end "${symbols}" __heap_end__)
    math(EXPR heap_size "${heap_end} - ${heap_base}")
    pad_left(heap_text 7 ${heap_size})
    string(APPEND lines "heap   ${heap_text} bytes of ram0, not in the ram0 use\n")
    set(${variable} "${lines}" PARENT_SCOPE)
endfunction()

# largest_symbols(<variable> <elf file>)
#
# Sets <variable> to the table of the largest symbols in flash and RAM
function(largest_symbols variable elf_file)
    execute_process(COMMAND ${NM_PROGRAM} --size-sort --reverse-sort --print-size
            --radix=d ${elf_file}
        OUTPUT_VARIABLE symbols COMMAND_ERROR_IS_FATAL ANY)
    string(REPLACE "\n" ";" symbol_lines "${symbols}")
    list(SUBLIST symbol_lines 0 ${LIST_LENGTH} symbol_lines)
    set(lines "  Bytes  Type  Symbol\n")
    foreach(symbol_line IN LISTS symbol_lines)
        string(REGEX MATCH "^[0-9]+ 0*([0-9]+) ([A-Za-z]) (.+)$" match "${symbol_line}")
        pad_left(size_text 7 ${CMAKE_MATCH_1})
        string(APPEND lines "${size_text}  ${CMAKE_MATCH_2}     ${CMAKE_MATCH_3}\n")
    endforeach()
    set(${variable} "${lines}" PARENT_SCOPE)
endfunction()

# largest_stack_frames(<variable> <stack usage folder>)
#
# Sets <variable> to the table of the largest stack frames in the .su files of gcc.
# A static inline function of a header is in each file that includes it, so equal
# lines go into the table one time
function(largest_stack_frames variable stack_usage_directory)
    file(GLOB_RECURSE stack_usage_files ${stack_usage_directory}/*.su)
    if(stack_usage_files STREQUAL "")
        message(FATAL_ERROR "No stack usage file in ${stack_usage_directory}")
    endif()
    set(frame_pattern "^(.+):([0-9]+):[0-9]+:([^\t]+)\t([0-9]+)\t(.+)$")
    set(frames "")
    foreach(stack_usage_file IN LISTS stack_usage_files)
        file(STRINGS ${stack_usage_file} frame_lines)
        foreach(frame_line IN LISTS frame_lines)
            string(REGEX MATCH "${frame_pattern}" match "${frame_line}")
            set(source_file "${CMAKE_MATCH_1}")
            set(location "${CMAKE_MATCH_1}:${CMAKE_MATCH_2}")
            set(function "${CMAKE_MATCH_3}")
            set(kind "${CMAKE_MATCH_5}")
            # All byte texts have the same width, so they sort as numbers
            pad_left(bytes_text 7 ${CMAKE_MATCH_4})
            cmake_path(IS_PREFIX REPOSITORY "${source_file}" in_repository)
            if(in_repository)
                cmake_path(RELATIVE_PATH location BASE_DIRECTORY ${REPOSITORY})
            endif()
            list(APPEND frames "${bytes_text}\t${kind}\t${function}\t${location}")
        endforeach()
    endforeach()
    list(REMOVE_DUPLICATES frames)
    list(SORT frames ORDER DESCENDING)
    list(SUBLIST frames 0 ${LIST_LENGTH} frames)

    set(lines "  Bytes  Kind             Function                          Location\n")
    foreach(frame IN LISTS frames)
        string(REPLACE "\t" ";" fields "${frame}")
        list(GET fields 0 bytes_text)
        list(GET fields 1 kind)
        list(GET fields 2 function)
        list(GET fields 3 location)
        pad_right(kind_text 15 "${kind}")
        pad_right(function_text 32 "${function}")
        string(APPEND lines
            "${bytes_text}  ${kind_text}  ${function_text}  ${location}\n")
    endforeach()
    set(${variable} "${lines}" PARENT_SCOPE)
endfunction()

set(report "")
foreach(build_name IN LISTS BUILD_NAMES)
    file(GLOB elf_files ${QMK_BUILD_ROOT}/${build_name}/*.elf)
    list(LENGTH elf_files elf_count)
    if(NOT elf_count EQUAL 1)
        message(FATAL_ERROR "Expected one .elf file in "
            "${QMK_BUILD_ROOT}/${build_name}, found ${elf_count}. "
            "Build the firmware first")
    endif()
    cmake_path(RELATIVE_PATH elf_files BASE_DIRECTORY ${REPOSITORY}
        OUTPUT_VARIABLE elf_name)

    memory_use(memory_lines ${elf_files})
    largest_symbols(symbol_lines ${elf_files})
    largest_stack_frames(frame_lines ${STACK_USAGE_ROOT}/${build_name})
    string(APPEND report
        "Build ${build_name}: ${elf_name}\n\n"
        "${memory_lines}\n"
        "Largest symbols\n${symbol_lines}\n"
        "Largest stack frames\n${frame_lines}\n")
endforeach()

file(WRITE ${REPORT_FILE} "${report}")
cmake_path(RELATIVE_PATH REPORT_FILE BASE_DIRECTORY ${REPOSITORY}
    OUTPUT_VARIABLE report_name)
message("${report}memory report ${report_name}")
