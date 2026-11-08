#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${MULL_SHA256:?not set or empty}"
: "${MULL_PLUGIN_SHA256:?not set or empty}"

# A restored cache can also contain a changed file, so each run checks the hashes
sha256sum -c - <<EOF
$MULL_SHA256  $DEB_DIR/mull.deb
$MULL_PLUGIN_SHA256  $DEB_DIR/mull-plugin.deb
EOF
