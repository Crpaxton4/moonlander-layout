#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${PR_NUMBER:?not set or empty}"
: "${FIRMWARE_PATHS:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

files=$(gh pr diff "$PR_NUMBER" --repo "$GITHUB_REPOSITORY" --name-only)
echo "firmware=$(grep -qE "$FIRMWARE_PATHS" <<<"$files" && echo true || echo false)" \
    >>"$GITHUB_OUTPUT"
