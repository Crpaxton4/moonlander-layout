#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${RELEASE_PR:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"

number=$(jq -er .number <<<"$RELEASE_PR")
gh pr merge --auto --squash --repo "$GITHUB_REPOSITORY" "$number"
