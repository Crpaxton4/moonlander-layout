#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${SHELLCHECK_VERSION:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

key="shellcheck-$RUNNER_OS-$ImageOS-$ImageVersion"
key="$key-$SHELLCHECK_VERSION"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
