#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"

expected="$RUNNER_TEMP/shellcheck-root/usr/bin/shellcheck"
actual=$(command -v shellcheck || true)
echo "shellcheck at $actual"
if [ "$actual" != "$expected" ]; then
    echo "::error::shellcheck on PATH is '$actual', not $expected"
    exit 1
fi
shellcheck --version
