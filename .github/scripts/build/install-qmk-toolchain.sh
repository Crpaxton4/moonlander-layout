#!/usr/bin/env bash
set -euo pipefail

: "${QMK_TOOLCHAINS_SHA256:?not set or empty}"
: "${QMK_FLASHUTILS_SHA256:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

sha256sum -c - <<EOF
$QMK_TOOLCHAINS_SHA256  qmk-assets/toolchains.tar.zst
$QMK_FLASHUTILS_SHA256  qmk-assets/flashutils.tar.zst
EOF
dir="$RUNNER_TEMP/qmk-distrib"
mkdir -p "$dir"
tar --zstd -xf qmk-assets/toolchains.tar.zst -C "$dir" \
    --strip-components=1 --exclude=avr --exclude='avr-*' \
    --exclude='riscv32-unknown-elf*'
tar --zstd -xf qmk-assets/flashutils.tar.zst -C "$dir/bin" ./dfu-suffix
# QMK CLI finds the toolchain in QMK_DISTRIB_DIR/bin
echo "QMK_DISTRIB_DIR=$dir" >>"$GITHUB_ENV"
echo "$dir/bin" >>"$GITHUB_PATH"
"$dir/bin/arm-none-eabi-gcc" --version
"$dir/bin/dfu-suffix" --version
