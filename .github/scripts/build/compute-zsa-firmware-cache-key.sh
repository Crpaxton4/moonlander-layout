#!/usr/bin/env bash
set -euo pipefail

: "${QMK_SUBMODULES:?not set or empty}"
: "${RUNNER_OS:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

ref=$(cat src/qmk/.qmk-ref)
submodules_hash=$(sha256sum <<<"$QMK_SUBMODULES" | cut -c1-16)
echo "ref=$ref" >>"$GITHUB_OUTPUT"
echo "key=qmk-firmware-$RUNNER_OS-$ref-$submodules_hash" | tee -a "$GITHUB_OUTPUT"
