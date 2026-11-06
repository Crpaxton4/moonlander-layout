#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${PR_NUMBER:?not set or empty}"
: "${RELEASE_ONLY_PATHS:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

files=$(gh pr diff "$PR_NUMBER" --repo "$GITHUB_REPOSITORY" --name-only)
echo "$files"
if [ -n "$files" ] && ! grep -qvE "$RELEASE_ONLY_PATHS" <<<"$files"; then
    echo "release-only=true" >>"$GITHUB_OUTPUT"
else
    echo "release-only=false" >>"$GITHUB_OUTPUT"
fi
