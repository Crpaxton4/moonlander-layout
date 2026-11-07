# Runs a program on one fixture case and compares its outputs with the expected files
# of the case. Each mismatch shows its diff, and the case fails
#
#   cmake -DPROGRAM=<program> "-DPROGRAM_ARGUMENTS=<argument>;..."
#       -DOUTPUT_DIRECTORY=<folder> "-DEXPECTED_FILES=<file>;..."
#       "-DACTUAL_FILES=<name>;..." -P run-fixture.cmake
#
# OUTPUT_DIRECTORY gets "stdout", with "exit <status>" as its last line, and "stderr".
# The program can write more files into it. Each entry of ACTUAL_FILES is a file name
# in OUTPUT_DIRECTORY, and the entry of EXPECTED_FILES at the same position gives its
# expected bytes. PROGRAM_ARGUMENTS can be empty
foreach(variable IN ITEMS PROGRAM OUTPUT_DIRECTORY EXPECTED_FILES ACTUAL_FILES)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()
list(LENGTH EXPECTED_FILES expected_count)
list(LENGTH ACTUAL_FILES actual_count)
if(NOT expected_count EQUAL actual_count)
    message(FATAL_ERROR "EXPECTED_FILES and ACTUAL_FILES have different lengths")
endif()

# Output of an earlier run must not hide a file that this run does not write
file(REMOVE_RECURSE "${OUTPUT_DIRECTORY}")
file(MAKE_DIRECTORY "${OUTPUT_DIRECTORY}")
execute_process(COMMAND "${PROGRAM}" ${PROGRAM_ARGUMENTS}
    OUTPUT_FILE "${OUTPUT_DIRECTORY}/stdout"
    ERROR_FILE "${OUTPUT_DIRECTORY}/stderr"
    RESULT_VARIABLE exit_status)
file(APPEND "${OUTPUT_DIRECTORY}/stdout" "exit ${exit_status}\n")

# Loop compares all files, so one run shows each wrong output
set(mismatch_count 0)
foreach(expected_file actual_name IN ZIP_LISTS EXPECTED_FILES ACTUAL_FILES)
    set(actual_file "${OUTPUT_DIRECTORY}/${actual_name}")
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
