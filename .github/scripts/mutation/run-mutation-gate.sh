#!/usr/bin/env bash
set -euo pipefail

: "${MUTATION_DIFF_REF:?not set or empty}"
: "${CMAKE_BUILD_PARALLEL_LEVEL:?not set or empty}"

# Mull runs with two workers each, so two runs on four cores keep each core busy.
# Programs whose tests run through CTest then also use two cores. The compile step
# uses all cores
cmake --preset mutation-diff "-DMUTATION_DIFF_REF=$MUTATION_DIFF_REF" \
    -DMUTATION_WORKERS=2
cmake --build --preset mutation-diff --target mutation-programs
time cmake --build --preset mutation-diff --parallel $((CMAKE_BUILD_PARALLEL_LEVEL / 2))
