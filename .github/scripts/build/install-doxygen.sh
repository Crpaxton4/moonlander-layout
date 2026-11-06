#!/usr/bin/env bash
set -euo pipefail

: "${DOXYGEN_VERSION:?not set or empty}"
: "${DOXYGEN_SHA256:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

tag="Release_$(echo "$DOXYGEN_VERSION" | tr . _)"
curl -fsSL -o "$RUNNER_TEMP/doxygen.tar.gz" \
    "https://github.com/doxygen/doxygen/releases/download/$tag/\
doxygen-$DOXYGEN_VERSION.linux.bin.tar.gz"
echo "$DOXYGEN_SHA256  $RUNNER_TEMP/doxygen.tar.gz" | sha256sum -c -
mkdir -p "$HOME/.local/bin"
tar -xzf "$RUNNER_TEMP/doxygen.tar.gz" -C "$HOME/.local/bin" \
    --strip-components=2 "doxygen-$DOXYGEN_VERSION/bin/doxygen"
# Release binary goes first on PATH, so no other doxygen hides it
echo "$HOME/.local/bin" >>"$GITHUB_PATH"
export PATH="$HOME/.local/bin:$PATH"
which -a doxygen
doxygen --version
