#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${MULL_VERSION:?not set or empty}"
: "${CLANG_VERSION:?not set or empty}"

# Mull plugin package names the LLVM release of the clang package, without the
# epoch and the Ubuntu revision. Runner is Ubuntu 24.04, so the packages are for
# that release
llvm_release=${CLANG_VERSION#*:}
llvm_release=${llvm_release%%-*}
llvm_major=${llvm_release%%.*}
url="https://github.com/mull-project/mull/releases/download/$MULL_VERSION"
mkdir -p "$DEB_DIR"
curl -fsSL -o "$DEB_DIR/mull.deb" "$url/Mull-$MULL_VERSION-ubuntu-amd64-24.04.deb"
curl -fsSL -o "$DEB_DIR/mull-plugin.deb" \
    "$url/Mull-$llvm_major-$MULL_VERSION-LLVM-$llvm_release-ubuntu-amd64-24.04.deb"
