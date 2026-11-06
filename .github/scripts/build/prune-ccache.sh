#!/usr/bin/env bash
set -euo pipefail

: "${QMK_PATH_PREFIX:?not set or empty}"
: "${CCACHE_DIR:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

ccache="$QMK_PATH_PREFIX/ccache"
"$ccache" --show-stats --verbose
writes=$("$ccache" --print-stats | awk '$1 == "local_storage_write" { print $2 }')
# Eviction keeps only the objects of this build
start=$(stat -c %Y "$RUNNER_TEMP/ccache-build-start")
"$ccache" --evict-older-than "$(($(date +%s) - start + 1))s"
du -sh "$RUNNER_TEMP/ccache"
echo "save=$([ "${writes:-0}" -gt 0 ] && echo true || echo false)" \
    | tee -a "$GITHUB_OUTPUT"
