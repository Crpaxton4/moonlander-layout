#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${RUNNER_ARCH:?not set or empty}"
: "${RUNNER_TOOL_CACHE:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

version=$(cat .node-version)
if ! [[ "$version" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "::error::.node-version is '$version', not an exact release"
    exit 1
fi
key="node-$RUNNER_OS-$RUNNER_ARCH-$ImageOS-$ImageVersion"
key="$key-$version"
echo "version=$version" | tee -a "$GITHUB_OUTPUT"
echo "path=$RUNNER_TOOL_CACHE/node/$version" | tee -a "$GITHUB_OUTPUT"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
