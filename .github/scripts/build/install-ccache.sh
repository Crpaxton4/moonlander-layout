#!/usr/bin/env bash
set -euo pipefail

: "${QMK_DISTRIB_DIR:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"

# shared/install-ccache.sh puts ccache into this folder. Compile database must
# record the ccache link as the compiler for clang-tidy. USE_CCACHE of QMK records
# `ccache` instead
bin="$RUNNER_TEMP/ccache-bin"
ln -sf ccache "$bin/arm-none-eabi-gcc"
{
    echo "QMK_PATH_PREFIX=$bin"
    echo "CCACHE_PATH=$QMK_DISTRIB_DIR/bin"
} | tee -a "$GITHUB_ENV"
