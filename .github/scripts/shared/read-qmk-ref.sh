#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_OUTPUT:?not set or empty}"

echo "ref=$(cat src/qmk/.qmk-ref)" | tee -a "$GITHUB_OUTPUT"
