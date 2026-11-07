#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_WORKSPACE:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

{
    echo "## Memory report"
    echo
    echo '```text'
    cat "$GITHUB_WORKSPACE/.build/memory-report.txt"
    echo '```'
} >>"$GITHUB_STEP_SUMMARY"
