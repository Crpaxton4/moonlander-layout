#!/usr/bin/env bash
set -euo pipefail

: "${FIRMWARE_ONLY:?not set or empty}"
# Empty values are valid: a push run has no PR, and only a release has a tag
: "${RELEASE_TAG?not set}"
: "${PR_NUMBER?not set}"
: "${PR_HEAD_SHA?not set}"

# A PR checkout is a merge commit, so a PR build uses the PR head commit in the
# firmware name
if [ -n "$RELEASE_TAG" ]; then
    export FIRMWARE_SUFFIX="-$RELEASE_TAG"
elif [ -n "$PR_NUMBER" ]; then
    export FIRMWARE_SUFFIX="-pr$PR_NUMBER-${PR_HEAD_SHA:0:7}"
fi
# Firmware preset configures only the firmware part, so a release build needs no
# tree-sitter and no cmocka. The compile database build also builds the firmware
if [ "$FIRMWARE_ONLY" = true ]; then
    cmake --preset firmware
    cmake --build --preset firmware --target compile-databases
else
    cmake --workflow --preset build
fi
