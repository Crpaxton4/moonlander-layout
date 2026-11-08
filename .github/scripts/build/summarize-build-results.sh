#!/usr/bin/env bash
set -euo pipefail

: "${BUILD_OUTCOME:?not set or empty}"
: "${TEST_OUTCOME:?not set or empty}"
: "${ANALYSIS_OUTCOME:?not set or empty}"
: "${WINDOWS_OUTCOME:?not set or empty}"
: "${GITHUB_STEP_SUMMARY:?not set or empty}"

# Build check holds the build, the tests and the analysis, so the summary shows the
# part that failed
{
    echo "## Results"
    echo
    echo "| Part | Result |"
    echo "| --- | --- |"
    echo "| Build | $BUILD_OUTCOME |"
    echo "| Tests | $TEST_OUTCOME |"
    echo "| Static analysis | $ANALYSIS_OUTCOME |"
    echo "| Windows converter | $WINDOWS_OUTCOME |"
} | tee -a "$GITHUB_STEP_SUMMARY"
