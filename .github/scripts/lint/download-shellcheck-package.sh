#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${SHELLCHECK_VERSION:?not set or empty}"

sudo apt-get update
mkdir -p "$DEB_DIR"
cd "$DEB_DIR"
apt-get download "shellcheck=$SHELLCHECK_VERSION"
