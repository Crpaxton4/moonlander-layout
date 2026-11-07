#!/usr/bin/env bash
set -euo pipefail

: "${CMAKE_VERSION:?not set or empty}"
: "${CMOCKA_VERSION:?not set or empty}"

# Runner images ship a newer cmake in /usr/local/bin. make qmk-test calls the
# /usr/bin/cmake of this package
sudo apt-get update
sudo apt-get install -y --allow-downgrades "cmake=$CMAKE_VERSION" \
    "libcmocka-dev=$CMOCKA_VERSION"
/usr/bin/cmake --version
