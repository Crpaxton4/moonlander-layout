# Runs a program on one fixture case and compares its outputs with the expected files
# of the case. Each mismatch shows its diff, and the case fails
#
#   cmake -DPROGRAM=<program> "-DPROGRAM_ARGUMENTS=<argument>;..."
#       -DSOURCE_DIRECTORY=<folder> "-DINPUT_FILES=<file>;..."
#       -DRUN_DIRECTORY=<folder> -DOUTPUT_DIRECTORY=<folder>
#       "-DEXPECTED_FILES=<file>;..." "-DACTUAL_FILES=<name>;..." -P run-fixture.cmake
#
# Program runs in RUN_DIRECTORY, an absolute path in the build directory. The script
# makes this folder again for each run. Each entry of INPUT_FILES is a path relative to
# SOURCE_DIRECTORY, and the script copies that file to the same relative path in
# RUN_DIRECTORY. Thus each argument can be a relative path. A mutant that cuts an
# output path short then writes into the run folder, not into the source tree.
#
# OUTPUT_DIRECTORY is a relative path in RUN_DIRECTORY. It gets "stdout", with
# "exit <status>" as its last line, and "stderr". The program can write more files
# into it. Each entry of ACTUAL_FILES is a file name in OUTPUT_DIRECTORY, and the entry
# of EXPECTED_FILES at the same position gives its expected bytes. PROGRAM_ARGUMENTS
# and INPUT_FILES can be empty
foreach(variable IN ITEMS PROGRAM SOURCE_DIRECTORY RUN_DIRECTORY OUTPUT_DIRECTORY
        EXPECTED_FILES ACTUAL_FILES)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()
if(NOT IS_ABSOLUTE "${RUN_DIRECTORY}")
    message(FATAL_ERROR "RUN_DIRECTORY is not an absolute path: ${RUN_DIRECTORY}")
endif()
if(IS_ABSOLUTE "${OUTPUT_DIRECTORY}")
    message(FATAL_ERROR "OUTPUT_DIRECTORY is not a relative path: ${OUTPUT_DIRECTORY}")
endif()
list(LENGTH EXPECTED_FILES expected_count)
list(LENGTH ACTUAL_FILES actual_count)
if(NOT expected_count EQUAL actual_count)
    message(FATAL_ERROR "EXPECTED_FILES and ACTUAL_FILES have different lengths")
endif()

# Output of an earlier run must not hide a file that this run does not write
file(REMOVE_RECURSE "${RUN_DIRECTORY}")
set(output_folder "${RUN_DIRECTORY}/${OUTPUT_DIRECTORY}")
file(MAKE_DIRECTORY "${output_folder}")
# file(COPY) keeps a symbolic link as a link, also a link to a file that does not exist
foreach(input_file IN LISTS INPUT_FILES)
    cmake_path(GET input_file PARENT_PATH input_folder)
    file(COPY "${SOURCE_DIRECTORY}/${input_file}"
        DESTINATION "${RUN_DIRECTORY}/${input_folder}")
endforeach()
execute_process(COMMAND "${PROGRAM}" ${PROGRAM_ARGUMENTS}
    WORKING_DIRECTORY "${RUN_DIRECTORY}"
    OUTPUT_FILE "${output_folder}/stdout"
    ERROR_FILE "${output_folder}/stderr"
    RESULT_VARIABLE exit_status)
file(APPEND "${output_folder}/stdout" "exit ${exit_status}\n")

# Loop compares all files, so one run shows each wrong output
set(mismatch_count 0)
foreach(expected_file actual_name IN ZIP_LISTS EXPECTED_FILES ACTUAL_FILES)
    set(actual_file "${output_folder}/${actual_name}")
    execute_process(
        COMMAND "${CMAKE_COMMAND}" -E compare_files "${expected_file}" "${actual_file}"
        RESULT_VARIABLE compare_result)
    if(NOT compare_result EQUAL 0)
        execute_process(COMMAND diff -u "${expected_file}" "${actual_file}")
        math(EXPR mismatch_count "${mismatch_count} + 1")
    endif()
endforeach()
if(mismatch_count GREATER 0)
    message(FATAL_ERROR "${mismatch_count} of ${expected_count} outputs do not match")
endif()
