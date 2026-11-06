#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${PR_NUMBER:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"

gh pr merge --auto --squash --repo "$GITHUB_REPOSITORY" "$PR_NUMBER"
