#!/usr/bin/env bash
set -euo pipefail

: "${GCC_STRICT_DATABASE:?not set or empty}"

# Each file compiles also after a failed file, so each finding shows in one run
status=0
count=$(jq length "$GCC_STRICT_DATABASE")
for ((index = 0; index < count; index++)); do
    directory=$(jq -r ".[$index].directory" "$GCC_STRICT_DATABASE")
    mapfile -d '' -t arguments < <(jq --raw-output0 ".[$index].arguments[]" \
        "$GCC_STRICT_DATABASE")
    (cd "$directory" && "${arguments[@]}") || status=1
done
exit "$status"
