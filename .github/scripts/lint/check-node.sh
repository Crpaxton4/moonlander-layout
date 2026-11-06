#!/usr/bin/env bash
set -euo pipefail

: "${NODE_VERSION:?not set or empty}"
: "${NODE_DIR:?not set or empty}"

actual=$(node --version || true)
path=$(command -v node || true)
echo "node $actual at $path"
if [ "$actual" != "v$NODE_VERSION" ]; then
    echo "::error::node is '$actual', not the pinned v$NODE_VERSION"
    exit 1
fi
if [[ "$path" != "$NODE_DIR"/* ]]; then
    echo "::error::node on PATH is '$path', not in $NODE_DIR"
    exit 1
fi
# npm comes with the node release, so the script checks only node
echo "npm $(npm --version)"
npm ls
