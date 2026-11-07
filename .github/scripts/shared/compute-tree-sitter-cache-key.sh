#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${TREE_SITTER_VERSION:?not set or empty}"
: "${TREE_SITTER_C_VERSION:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

key="tree-sitter-$RUNNER_OS-$ImageOS-$ImageVersion"
key="$key-$TREE_SITTER_VERSION-$TREE_SITTER_C_VERSION"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
