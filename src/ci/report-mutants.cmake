# Shows the result of all Mull runs of a mutation build with mull-reporter. Without
# --allow-surviving in REPORTER_OPTIONS, each surviving mutant fails the script
#
#   cmake -DREPORTER=<mull-reporter> -DCONFIG=<mull.yml> -DDATABASE=<file>
#       -DREPORT_DIRECTORY=<folder> "-DREPORTER_OPTIONS=<option>;..."
#       -P report-mutants.cmake
#
# REPORTER_OPTIONS can be empty
foreach(variable IN ITEMS REPORTER CONFIG DATABASE REPORT_DIRECTORY)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

# mull-runner writes no database if a diff run has no mutant
if(NOT EXISTS "${DATABASE}")
    message("No mutants: the changes contain no mutable code under test")
    return()
endif()

execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "MULL_CONFIG=${CONFIG}"
        "${REPORTER}" "${DATABASE}" --reporters IDE --reporters Elements
        --report-dir "${REPORT_DIRECTORY}" --report-name mutation ${REPORTER_OPTIONS}
    RESULT_VARIABLE reporter_result)
# IDE reporter writes the surviving mutants into a file in the report folder, so the
# log shows that file
execute_process(COMMAND "${CMAKE_COMMAND}" -E cat
    "${REPORT_DIRECTORY}/mutation.txt")
if(NOT reporter_result EQUAL 0)
    message(FATAL_ERROR "Mutation gate failed: no test detects each surviving "
        "mutant above. Add a test that fails for that change of the code")
endif()
