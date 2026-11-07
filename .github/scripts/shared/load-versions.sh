#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_ENV:?not set or empty}"

# GITHUB_ENV takes NAME=value lines like versions.env, but no comments
grep -v -e '^#' -e '^$' versions.env | tee -a "$GITHUB_ENV"
