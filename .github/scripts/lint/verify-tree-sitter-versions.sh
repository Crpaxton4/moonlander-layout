#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${TREE_SITTER_VERSION:?not set or empty}"
: "${TREE_SITTER_C_VERSION:?not set or empty}"

# Script checks all packages before it fails, so the log shows each wrong
# version
status=0
verify() {
    actual=$(dpkg-deb -f "$DEB_DIR/$1"_*.deb Version || true)
    echo "$1 $actual"
    if [ "$actual" != "$2" ]; then
        echo "::error::$1 is '$actual', not the pinned $2"
        status=1
    fi
}
verify libtree-sitter0 "$TREE_SITTER_VERSION"
verify libtree-sitter-dev "$TREE_SITTER_VERSION"
verify tree-sitter-c-src "$TREE_SITTER_C_VERSION"
exit "$status"
