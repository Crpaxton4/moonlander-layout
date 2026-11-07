#!/usr/bin/env bash
set -euo pipefail

: "${REPORT_DIR:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

# Coverage gate writes one Markdown report for each area. A failed build or test
# step writes none
shopt -s nullglob
for report in "$REPORT_DIR"/*.md; do
    cat "$report" >>"$GITHUB_STEP_SUMMARY"
done
