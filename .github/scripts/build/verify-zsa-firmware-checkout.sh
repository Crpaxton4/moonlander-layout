#!/usr/bin/env bash
set -euo pipefail

: "${QMK_SUBMODULES:?not set or empty}"
: "${QMK_REF:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"

read -ra submodules <<<"$QMK_SUBMODULES"
head=$(git rev-parse HEAD)
echo "HEAD $head"
if [ "$head" != "$QMK_REF" ]; then
    echo "::error::qmk_firmware is at $head, not at the pin $QMK_REF"
    exit 1
fi
# `git submodule status` starts the line of a submodule at its recorded commit
# with a space
git submodule status -- "${submodules[@]}" | tee "$RUNNER_TEMP/submodules.txt"
status=0
for path in "${submodules[@]}"; do
    recorded=$(git rev-parse "HEAD:$path")
    if ! grep -qE "^ $recorded $path( |$)" "$RUNNER_TEMP/submodules.txt"; then
        echo "::error::$path is not at $recorded"
        status=1
    fi
done
exit "$status"
