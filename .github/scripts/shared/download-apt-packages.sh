#!/usr/bin/env bash
set -euo pipefail

: "${APT_PACKAGES:?not set or empty}"
: "${DEB_DIR:?not set or empty}"

read -ra packages <<<"$APT_PACKAGES"
sudo apt-get update
mkdir -p "$DEB_DIR"
# apt checks the signature of the archive and the hash of each file. It downloads
# each pinned package and each dependency that the runner image does not contain
sudo apt-get install --download-only --yes --no-install-recommends \
    --allow-downgrades -o "Dir::Cache::archives=$DEB_DIR" "${packages[@]}"
sudo rm -rf "$DEB_DIR/partial" "$DEB_DIR/lock"
# A package that the image contains at its pin gives no file. The list of pins
# makes sure that the cache folder is never empty
printf '%s\n' "${packages[@]}" >"$DEB_DIR/pins.txt"
ls -l "$DEB_DIR"
