#!/usr/bin/env bash
set -euo pipefail

: "${MUTATION_DIFF_REF:?not set or empty}"
: "${CMAKE_BUILD_PARALLEL_LEVEL:?not set or empty}"

# The build starts one Mull run for each core at the same time. Each run has one
# worker, so each core runs the tests of one mutant at a time
cmake --preset mutation-diff "-DMUTATION_DIFF_REF=$MUTATION_DIFF_REF" \
    -DMUTATION_WORKERS=1
time cmake --build --preset mutation-diff
