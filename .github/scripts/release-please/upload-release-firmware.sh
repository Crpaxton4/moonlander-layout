#!/usr/bin/env bash
set -euo pipefail

: "${GH_TOKEN:?not set or empty}"
: "${TAG:?not set or empty}"
: "${GITHUB_REPOSITORY:?not set or empty}"

# Stops the release when the artifact does not hold exactly one firmware file
# with the tag in its name
shopt -s nullglob
files=(./*.bin)
if [ "${#files[@]}" -ne 1 ] || [[ "${files[0]}" != *"-$TAG.bin" ]]; then
    echo "::error::Expected one .bin that ends in -$TAG.bin, found: ${files[*]}"
    exit 1
fi
gh release upload --repo "$GITHUB_REPOSITORY" "$TAG" "${files[0]}"
