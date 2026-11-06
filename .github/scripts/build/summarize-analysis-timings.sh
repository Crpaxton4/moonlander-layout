#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

[ -f "$RUNNER_TEMP/analyze.log" ] || exit 0
{
    echo "| Check | Duration |"
    echo "| --- | --- |"
    awk '
        $1 == "timing" && $2 == "analyze-total" {
            print "| **Total** | " $3 " |"
            next
        }
        $1 == "timing" { print "| " $2 " | " $3 " |" }
    ' "$RUNNER_TEMP/analyze.log"
} >>"$GITHUB_STEP_SUMMARY"
