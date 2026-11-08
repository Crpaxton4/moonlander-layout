#!/usr/bin/env bash
set -euo pipefail

: "${REPORT_DIR:?not set or empty}"
: "${MULL_CONFIG:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

# mull-runner writes no database if no mutant ran, for example in a PR with no change
# of the code under test
if [ ! -f "$REPORT_DIR/mutation.sqlite" ]; then
    echo "No mutant ran." >>"$GITHUB_STEP_SUMMARY"
    exit 0
fi
# Each annotation shows a surviving mutant at its line in the PR diff
mull-reporter "$REPORT_DIR/mutation.sqlite" --reporters GitHubAnnotations \
    --allow-surviving
{
    echo '```text'
    cat "$REPORT_DIR/mutation.txt"
    echo '```'
} >>"$GITHUB_STEP_SUMMARY"
