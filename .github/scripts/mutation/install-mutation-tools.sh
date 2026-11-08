#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${CLANG_VERSION:?not set or empty}"

# Package names of clang contain the LLVM major version. The sanitizers of the
# mutation build need the compiler-rt package
llvm_release=${CLANG_VERSION#*:}
llvm_major=${llvm_release%%.*}
sudo apt-get update
# apt installs the dependencies of the Mull packages from the Ubuntu archive
sudo apt-get install -y --allow-downgrades "clang-$llvm_major=$CLANG_VERSION" \
    "libclang-rt-$llvm_major-dev=$CLANG_VERSION" \
    "$DEB_DIR/mull.deb" "$DEB_DIR/mull-plugin.deb"
"clang-$llvm_major" --version
# mull-runner shows each plugin that it finds, with its LLVM release
mull-runner --version
