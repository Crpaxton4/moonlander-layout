# Each case directory holds <case>.expected, the expected checker output with the exit
# status. Arguments are the .c, .h and .inc files of the case in sort order. Names end
# in _first, _second or _third to set an order. Paths in the output start at the
# repository root, so the case runs there
file(GLOB case_expectations CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*/*.expected)
foreach(expected_file IN LISTS case_expectations)
    cmake_path(GET expected_file STEM case)
    set(case_directory ${CMAKE_CURRENT_SOURCE_DIR}/${case})
    file(GLOB case_files CONFIGURE_DEPENDS RELATIVE ${PROJECT_SOURCE_DIR}
        ${case_directory}/*.c ${case_directory}/*.h ${case_directory}/*.inc)
    list(SORT case_files)
    add_test(NAME check_standards.${case}
        COMMAND ${CMAKE_COMMAND} -DPROGRAM=$<TARGET_FILE:check_standards>
            "-DPROGRAM_ARGUMENTS=${case_files}"
            -DOUTPUT_DIRECTORY=${CMAKE_CURRENT_BINARY_DIR}/${case}
            -DEXPECTED_FILES=${expected_file} -DACTUAL_FILES=stdout
            -P ${PROJECT_SOURCE_DIR}/src/ci/run-fixture.cmake
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
    set_tests_properties(check_standards.${case} PROPERTIES LABELS checker)
endforeach()
