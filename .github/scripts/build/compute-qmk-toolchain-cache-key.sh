#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${QMK_TOOLCHAINS_RELEASE:?not set or empty}"
: "${QMK_FLASHUTILS_RELEASE:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

key="qmk-toolchain-$RUNNER_OS-$ImageOS-$ImageVersion"
key="$key-$QMK_TOOLCHAINS_RELEASE-$QMK_FLASHUTILS_RELEASE"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
