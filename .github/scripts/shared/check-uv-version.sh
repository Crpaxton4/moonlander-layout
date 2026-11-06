#!/usr/bin/env bash
set -euo pipefail

: "${UV_VERSION:?not set or empty}"

actual=$(uv --version || true)
echo "$actual"
read -r _ version _ <<<"$actual"
if [ "$version" != "$UV_VERSION" ]; then
    echo "::error::uv is '$version', not the pinned $UV_VERSION"
    exit 1
fi
