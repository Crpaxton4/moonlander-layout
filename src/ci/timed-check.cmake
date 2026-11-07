# Runs one check command and prints "timing <CHECK_NAME> <seconds>s", also when the
# check fails. The CI timing summary reads these lines
#
#   cmake -DCHECK_NAME=<name> "-DCHECK_COMMAND=<command>;<argument>..."
#       -P timed-check.cmake
foreach(variable IN ITEMS CHECK_NAME CHECK_COMMAND)
    if("${${variable}}" STREQUAL "")
        message(FATAL_ERROR "${variable} is not set or empty")
    endif()
endforeach()

# %f gives the microseconds, so the duration has the same three decimals as the
# TIMEFORMAT of bash
string(TIMESTAMP start_time "%s%f" UTC)
execute_process(COMMAND ${CHECK_COMMAND} RESULT_VARIABLE check_result)
string(TIMESTAMP end_time "%s%f" UTC)

math(EXPR milliseconds "(${end_time} - ${start_time}) / 1000")
math(EXPR seconds "${milliseconds} / 1000")
math(EXPR fraction "${milliseconds} % 1000 + 1000")
string(SUBSTRING "${fraction}" 1 3 fraction)
message("timing ${CHECK_NAME} ${seconds}.${fraction}s")

if(NOT check_result EQUAL 0)
    message(FATAL_ERROR "${CHECK_NAME} failed: ${check_result}")
endif()
