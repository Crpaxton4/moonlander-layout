#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"

# dpkg -i takes up to 20 s on the runner, so the script only extracts the
# packages
root="$RUNNER_TEMP/tree-sitter-root"
for package in libtree-sitter0 libtree-sitter-dev tree-sitter-c-src; do
    dpkg-deb -x "$DEB_DIR/$package"_*.deb "$root"
done
# CMake finds the headers, the library and the grammar below this prefix
echo "CMAKE_PREFIX_PATH=$root/usr" >>"$GITHUB_ENV"
