# Each host program and each unit test gets these flags, so all C code of the
# repository has the same strict rules. Every warning blocks the build
set(STRICT_C_FLAGS
    -std=c11
    -Werror
    # Usual warning sets, and ISO C only
    -Wall
    -Wextra
    -Wpedantic
    # Changes of value or sign in implicit conversions
    -Wconversion
    -Wsign-conversion
    # Declarations
    -Wshadow
    -Wstrict-prototypes
    -Wmissing-prototypes
    -Wmissing-declarations
    -Wold-style-definition
    -Wredundant-decls
    # Casts and pointers
    -Wcast-qual
    -Wcast-align
    -Wbad-function-cast
    -Wpointer-arith
    -Wwrite-strings
    # Preprocessor, floating point and formats
    -Wundef
    -Wdouble-promotion
    -Wfloat-equal
    -Wformat=2
    # Control flow and memory
    -Wswitch-enum
    -Wvla
    -Walloca
    -Wnull-dereference
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wjump-misses-init
    # GCC path-based static analysis
    -fanalyzer)
