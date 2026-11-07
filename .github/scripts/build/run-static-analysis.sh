#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"

# Keymap checks read the compile database of the build step. The analyze build
# preset continues after a failed check, so a failed check does not hide other
# findings. The timing summary reads the total from TIMEFORMAT
TIMEFORMAT='timing analyze-total %Rs'
{ time cmake --workflow --preset analyze; } 2>&1 | tee "$RUNNER_TEMP/analyze.log"
