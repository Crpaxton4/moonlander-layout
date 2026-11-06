#!/usr/bin/env bash
set -euo pipefail

: "${TITLE:?not set or empty}"

echo "$TITLE" | npx --no -- commitlint
