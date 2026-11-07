#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${TREE_SITTER_VERSION:?not set or empty}"
: "${TREE_SITTER_C_VERSION:?not set or empty}"

sudo apt-get update
mkdir -p "$DEB_DIR"
cd "$DEB_DIR"
# libtree-sitter-dev needs libtree-sitter0 at the same version, so
# TREE_SITTER_VERSION pins both
apt-get download "libtree-sitter0=$TREE_SITTER_VERSION" \
    "libtree-sitter-dev=$TREE_SITTER_VERSION" \
    "tree-sitter-c-src=$TREE_SITTER_C_VERSION"
