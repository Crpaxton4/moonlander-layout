#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${CLANG_VERSION:?not set or empty}"
: "${GITHUB_WORKSPACE:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"

# Package names of clang contain the LLVM major version. The apt step installs the
# clang packages, and the Mull packages need no other package
llvm_release=${CLANG_VERSION#*:}
llvm_major=${llvm_release%%.*}
sudo dpkg --install --force-unsafe-io "$DEB_DIR/mull.deb" "$DEB_DIR/mull-plugin.deb"
"clang-$llvm_major" --version
# mull-runner shows each plugin that it finds, with its LLVM release
mull-runner --version

# ccache does not hash the plugin and the plugin configuration, but both change the
# mutants in each object. CMake finds the plugin in the lib directory next to the bin
# directory of mull-runner
prefix=$(dirname "$(dirname "$(command -v mull-runner)")")
plugin="$prefix/lib/mull-ir-frontend-$llvm_major"
ls -l "$plugin"
echo "CCACHE_EXTRAFILES=$plugin:$GITHUB_WORKSPACE/src/ci/mull.yml" \
    | tee -a "$GITHUB_ENV"
