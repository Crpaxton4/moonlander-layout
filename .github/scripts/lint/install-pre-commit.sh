#!/usr/bin/env bash
set -euo pipefail

: "${PRE_COMMIT_VERSION:?not set or empty}"

uv tool install "pre-commit==$PRE_COMMIT_VERSION"
