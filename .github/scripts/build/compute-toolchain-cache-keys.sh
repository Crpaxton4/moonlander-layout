#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${QMK_TOOLCHAINS_RELEASE:?not set or empty}"
: "${QMK_FLASHUTILS_RELEASE:?not set or empty}"
: "${CCACHE_VERSION:?not set or empty}"
: "${MINGW_VERSION:?not set or empty}"
: "${FLAGS_HASH:?not set or empty}"
: "${GITHUB_RUN_ID:?not set or empty}"
: "${GITHUB_RUN_ATTEMPT:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

key="qmk-toolchain-$RUNNER_OS-$ImageOS-$ImageVersion"
key="$key-$QMK_TOOLCHAINS_RELEASE-$QMK_FLASHUTILS_RELEASE"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"

# ccache hashes each compiler and all flags, so its keys have no image version. The
# QMK build, the host programs and the Windows converter share one ccache. The
# firmware prefix also restores an entry of other host flags, because the firmware
# objects do not use these flags
firmware_prefix="ccache-$RUNNER_OS-$ImageOS-$QMK_TOOLCHAINS_RELEASE-$CCACHE_VERSION-"
cc --version
host_version=$(cc -dumpfullversion)
host_hash=$(cc --version | sha256sum | cut -c1-8)
prefix="${firmware_prefix}cc-$host_version-$host_hash-mingw-$MINGW_VERSION"
prefix="$prefix-flags-${FLAGS_HASH:0:16}-"
{
    echo "ccache-firmware-prefix=$firmware_prefix"
    echo "ccache-prefix=$prefix"
    echo "ccache-key=$prefix$GITHUB_RUN_ID-$GITHUB_RUN_ATTEMPT"
} | tee -a "$GITHUB_OUTPUT"
