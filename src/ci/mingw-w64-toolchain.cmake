# Builds 64-bit Windows programs with the gcc-mingw-w64-x86-64-win32 package that
# versions.env pins. CMAKE_PREFIX_PATH can point to a `dpkg -x` extraction of it
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
find_program(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc-win32 REQUIRED)

# Program gets libgcc, so Windows needs no MinGW DLL
set(CMAKE_EXE_LINKER_FLAGS_INIT -static)
