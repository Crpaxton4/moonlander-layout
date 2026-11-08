#!/usr/bin/env bash
set -euo pipefail

: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

# Runner images ship a newer cmake in /usr/local/bin. A directory with links to the
# /usr/bin programs of the pinned package goes first on PATH for the next steps, so
# each cmake and ctest call uses the pinned version
links="$RUNNER_TEMP/cmake-bin"
mkdir -p "$links"
ln -sf /usr/bin/cmake /usr/bin/ctest "$links/"
echo "$links" >>"$GITHUB_PATH"
"$links/cmake" --version
