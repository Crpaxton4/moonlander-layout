#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

log="$RUNNER_TEMP/pre-commit.log"
[ -f "$log" ] || exit 0
{
    echo "### Hooks"
    echo
    echo "| Hook | Duration |"
    echo "| --- | --- |"
    awk '
        /\.\.\.(Passed|Failed)$/ {
            name = $0
            sub(/\.+(Passed|Failed)$/, "", name)
        }
        /^- duration: / { print "| " name " | " $3 " |" }
        /^real / { print "| **Total** | " $2 " |" }
    ' "$log"
    echo
} >>"$GITHUB_STEP_SUMMARY"
