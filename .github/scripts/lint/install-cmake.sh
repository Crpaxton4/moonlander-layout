#!/usr/bin/env bash
set -euo pipefail

: "${CMAKE_VERSION:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

# check-standards hook builds the checker with CMake. The checker needs no cmocka
sudo apt-get update
sudo apt-get install -y --allow-downgrades "cmake=$CMAKE_VERSION"

# Runner images ship a newer cmake in /usr/local/bin. A directory with a link to the
# /usr/bin/cmake of this package goes first on PATH for the next steps, so each
# cmake call uses the pinned version
links="$RUNNER_TEMP/cmake-bin"
mkdir -p "$links"
ln -sf /usr/bin/cmake "$links/"
echo "$links" >>"$GITHUB_PATH"
"$links/cmake" --version
