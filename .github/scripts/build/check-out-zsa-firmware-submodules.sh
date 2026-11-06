#!/usr/bin/env bash
set -euo pipefail

: "${QMK_SUBMODULES:?not set or empty}"

read -ra submodules <<<"$QMK_SUBMODULES"
git submodule update --init --depth 1 --jobs "${#submodules[@]}" -- "${submodules[@]}"
