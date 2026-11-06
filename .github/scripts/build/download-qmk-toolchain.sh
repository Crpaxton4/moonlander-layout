#!/usr/bin/env bash
set -euo pipefail

: "${QMK_TOOLCHAINS_RELEASE:?not set or empty}"
: "${QMK_FLASHUTILS_RELEASE:?not set or empty}"

# Asset names contain the GCC version: the tag without its `v` and its build
# number
gcc_version="${QMK_TOOLCHAINS_RELEASE#v}"
gcc_version="${gcc_version%-*}"
base="https://github.com/qmk"
mkdir -p qmk-assets
curl -fsSL -o qmk-assets/toolchains.tar.zst \
    "$base/qmk_toolchains/releases/download/$QMK_TOOLCHAINS_RELEASE/\
qmk_toolchains-gcc$gcc_version-host_linuxX64.tar.zst"
curl -fsSL -o qmk-assets/flashutils.tar.zst \
    "$base/qmk_flashutils/releases/download/$QMK_FLASHUTILS_RELEASE/\
qmk_flashutils-$QMK_FLASHUTILS_RELEASE-linuxX64.tar.zst"
