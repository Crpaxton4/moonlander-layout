#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"

# One run of all checks, so a failed check does not hide other findings. The build
# step made the compile database, so make does not build it again
make --assume-old=compiledb analyze 2>&1 | tee "$RUNNER_TEMP/analyze.log"
