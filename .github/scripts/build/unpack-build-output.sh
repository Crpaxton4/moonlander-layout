#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_WORKSPACE:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"

# Files go to the same paths as in the Build job, because the paths in them are
# absolute
tar -xvf "$RUNNER_TEMP/build-output.tar" -C "$GITHUB_WORKSPACE"
