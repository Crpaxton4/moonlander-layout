#!/usr/bin/env bash
set -euo pipefail

: "${LLVM_VERSION:?not set or empty}"

uv tool install "clang-tidy==$LLVM_VERSION"
clang-tidy --version
