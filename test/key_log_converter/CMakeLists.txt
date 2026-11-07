# Each case directory holds its logs as <case>*.log in sort order and the expected
# outputs as <case>.<output>, where <output> is a file name in the output folder of the
# run. stdout and stderr are outputs too. Paths in the output start at the repository
# root, so the case runs there
file(GLOB case_stdouts CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*/*.stdout)
foreach(case_stdout IN LISTS case_stdouts)
    cmake_path(GET case_stdout STEM case)
    set(case_directory ${CMAKE_CURRENT_SOURCE_DIR}/${case})
    file(GLOB case_logs CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}
        ${case_directory}/${case}*.log)
    list(SORT case_logs)

    file(GLOB expected_files CONFIGURE_DEPENDS ${case_directory}/${case}.*)
    list(FILTER expected_files EXCLUDE REGEX "\\.log$")
    set(actual_names "")
    string(LENGTH "${case}." prefix_length)
    foreach(expected_file IN LISTS expected_files)
        cmake_path(GET expected_file FILENAME expected_name)
        string(SUBSTRING "${expected_name}" ${prefix_length} -1 actual_name)
        list(APPEND actual_names ${actual_name})
    endforeach()

    set(output_directory ${CMAKE_CURRENT_BINARY_DIR}/${case})
    add_test(NAME key_log_converter.${case}
        COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:key_log_converter>
            "-DPROGRAM_ARGUMENTS=${output_directory};${case_logs}"
            -DOUTPUT_DIRECTORY=${output_directory}
            "-DEXPECTED_FILES=${expected_files}" "-DACTUAL_FILES=${actual_names}"
            -P ${PROJECT_SOURCE_DIR}/src/ci/run-fixture.cmake
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    set_tests_properties(key_log_converter.${case} PROPERTIES LABELS converter)
endforeach()
