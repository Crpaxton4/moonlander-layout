#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_ENV:?not set or empty}"

# cmake --build and ctest run one job at a time if nothing sets the number of jobs.
# All tests are independent, and each test that writes files has its own folder
cores=$(nproc)
{
    echo "CMAKE_BUILD_PARALLEL_LEVEL=$cores"
    echo "CTEST_PARALLEL_LEVEL=$cores"
} | tee -a "$GITHUB_ENV"
