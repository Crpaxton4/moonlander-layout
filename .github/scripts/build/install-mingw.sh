#!/usr/bin/env bash
set -euo pipefail

: "${MINGW_VERSION:?not set or empty}"

# Toolchain file of the windows CMake preset finds this compiler on PATH
sudo apt-get update
sudo apt-get install -y "gcc-mingw-w64-x86-64-win32=$MINGW_VERSION"
x86_64-w64-mingw32-gcc-win32 --version
