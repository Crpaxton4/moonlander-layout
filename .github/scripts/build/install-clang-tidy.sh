#!/usr/bin/env bash
set -euo pipefail

uv tool install "clang-tidy==$(cat .llvm-version)"
clang-tidy --version
