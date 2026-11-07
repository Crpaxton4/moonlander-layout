#!/usr/bin/env bash
set -euo pipefail

: "${GCOVR_VERSION:?not set or empty}"

uv tool install "gcovr==$GCOVR_VERSION"
gcovr --version
