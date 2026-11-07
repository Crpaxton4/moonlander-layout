# Checker needs the tree-sitter package versions that versions.env pins.
# CMAKE_PREFIX_PATH can point to a `dpkg -x` extraction of the packages
find_path(TREE_SITTER_INCLUDE_DIR tree_sitter/api.h REQUIRED)
find_library(TREE_SITTER_LIBRARY libtree-sitter.a REQUIRED)

# tree-sitter-c-src puts its parser in a directory with its version as name, below
# the same prefix as the headers
cmake_path(GET TREE_SITTER_INCLUDE_DIR PARENT_PATH tree_sitter_prefix)
file(GLOB grammar_directories
    ${tree_sitter_prefix}/src/tree-sitter/c/*/parser/src)
find_file(TREE_SITTER_C_GRAMMAR parser.c PATHS ${grammar_directories}
    NO_DEFAULT_PATH REQUIRED)

# Generated code, so the build ignores its warnings and does not analyze it
add_library(tree_sitter_c_grammar OBJECT ${TREE_SITTER_C_GRAMMAR})
set_property(TARGET tree_sitter_c_grammar PROPERTY COMPILE_OPTIONS -w)
target_include_directories(tree_sitter_c_grammar PRIVATE ${TREE_SITTER_INCLUDE_DIR})

set(CHECKER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rules.c
    ${CMAKE_CURRENT_SOURCE_DIR}/source.c
    ${CMAKE_CURRENT_SOURCE_DIR}/support.c)

# Hook runs the program from the top of the build directory
add_executable(check_standards ${CHECKER_SOURCES})
set_property(TARGET check_standards PROPERTY RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_BINARY_DIR})
target_include_directories(check_standards PRIVATE ${TREE_SITTER_INCLUDE_DIR})
target_link_libraries(check_standards PRIVATE tree_sitter_c_grammar
    ${TREE_SITTER_LIBRARY})

# CMake writes the compile database at the configure step, so clang-tidy does not
# need a build of the checker
add_custom_target(clang-tidy-checker
    COMMAND ${CMAKE_COMMAND} -DCHECK_NAME=clang-tidy-checker
        "-DCHECK_COMMAND=clang-tidy;--quiet;-p;${CMAKE_BINARY_DIR};${CHECKER_SOURCES}"
        -P ${PROJECT_SOURCE_DIR}/src/ci/timed-check.cmake
    VERBATIM)
add_dependencies(analyze clang-tidy-checker)
