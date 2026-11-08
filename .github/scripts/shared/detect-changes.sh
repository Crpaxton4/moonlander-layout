#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${PR_NUMBER:?not set or empty}"
: "${FIRMWARE_PATHS:?not set or empty}"
: "${UNIT_TEST_PATHS:?not set or empty}"
: "${RELEASE_ONLY_PATHS:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

files=$(gh pr diff "$PR_NUMBER" --repo "$GITHUB_REPOSITORY" --name-only)
echo "$files"
matches() {
    grep -qE "$1" <<<"$files" && echo true || echo false
}
# A PR is release-only if it changes files and each file is a release-please file
release_only=false
if [ -n "$files" ] && ! grep -qvE "$RELEASE_ONLY_PATHS" <<<"$files"; then
    release_only=true
fi
{
    echo "firmware=$(matches "$FIRMWARE_PATHS")"
    echo "unit-tests=$(matches "$UNIT_TEST_PATHS")"
    echo "release-only=$release_only"
} | tee -a "$GITHUB_OUTPUT"
