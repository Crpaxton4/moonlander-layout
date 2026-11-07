#!/usr/bin/env bash
set -euo pipefail

: "${CMAKE_VERSION:?not set or empty}"
: "${CMOCKA_VERSION:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

# Test job runs programs from the Build job, so it needs only ctest, the cmake that
# runs the fixture cases, and the cmocka library. The checker contains tree-sitter
sudo apt-get update
sudo apt-get install -y --allow-downgrades "cmake=$CMAKE_VERSION" \
    "libcmocka0=$CMOCKA_VERSION"

# Runner images ship a newer cmake in /usr/local/bin. A directory with links to the
# /usr/bin programs of this package goes first on PATH for the next steps, so each
# cmake and ctest call uses the pinned version
links="$RUNNER_TEMP/cmake-bin"
mkdir -p "$links"
ln -sf /usr/bin/cmake /usr/bin/ctest "$links/"
echo "$links" >>"$GITHUB_PATH"
"$links/ctest" --version
