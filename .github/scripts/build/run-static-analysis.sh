#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"
# Unit test part of the configure step finds the qmk_firmware checkout with this
# variable, so this job needs no QMK CLI
: "${QMK_HOME:?not set or empty}"

# Configure step writes the compile databases of the host programs and the unit
# tests. The keymap checks read the keymap compile database from the Build job.
# The analyze build preset continues after a failed check, so a failed check does
# not hide other findings. The timing summary reads the total from TIMEFORMAT
TIMEFORMAT='timing analyze-total %Rs'
{ time cmake --workflow --preset analyze; } 2>&1 | tee "$RUNNER_TEMP/analyze.log"
