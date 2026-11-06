#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_WORKSPACE:?not set or empty}"

qmk config user.qmk_home="$GITHUB_WORKSPACE/qmk_firmware" \
    user.overlay_dir="$GITHUB_WORKSPACE/src/qmk"
