#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"

# One run of all hooks, so a failed hook does not hide other findings
TIMEFORMAT='real %Rs'
{ time pre-commit run --all-files --hook-stage manual \
    --show-diff-on-failure --verbose; } 2>&1 \
    | tee "$RUNNER_TEMP/pre-commit.log"
