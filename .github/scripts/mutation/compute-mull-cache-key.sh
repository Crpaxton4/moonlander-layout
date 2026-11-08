#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${MULL_VERSION:?not set or empty}"
: "${MULL_SHA256:?not set or empty}"
: "${MULL_PLUGIN_SHA256:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

# Each package hash is in the key, so a new pin never restores an old package
key="mull-$RUNNER_OS-$MULL_VERSION-${MULL_SHA256:0:16}-${MULL_PLUGIN_SHA256:0:16}"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
