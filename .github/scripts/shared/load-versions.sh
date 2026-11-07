#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_ENV:?not set or empty}"

# GitHub reads each GITHUB_ENV line as a variable, so a bad line can set an
# unwanted variable. All lines must pass before the script writes one
pattern='^[A-Z][A-Z0-9_]*=[A-Za-z0-9._+~-]+$'
pins=()
while IFS= read -r line || [ -n "$line" ]; do
    case "$line" in
    '#'* | '') continue ;;
    esac
    if ! [[ "$line" =~ $pattern ]]; then
        echo "::error file=versions.env::'$line' is not a NAME=value pin"
        exit 1
    fi
    pins+=("$line")
done <versions.env
printf '%s\n' "${pins[@]}" | tee -a "$GITHUB_ENV"
