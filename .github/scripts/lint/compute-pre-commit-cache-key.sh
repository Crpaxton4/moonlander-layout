#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${PRE_COMMIT_VERSION:?not set or empty}"
: "${CONFIG_HASH:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

# pre-commit builds the hook environments with the Python and Go of the runner
# image, so the key contains the image
key="pre-commit-$RUNNER_OS-$ImageOS-$ImageVersion"
key="$key-$PRE_COMMIT_VERSION-$CONFIG_HASH"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
