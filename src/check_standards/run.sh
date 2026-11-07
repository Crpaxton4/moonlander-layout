#!/bin/sh
set -eu

repository="${0%/*}/../.."
build="$repository/.build/checker"

# Presets apply only in the source directory. Each build configures again when a
# CMake file changes, so the hook configures only while the generated Makefile is
# missing, also after a failed configure step. Only errors go to the output
if [ ! -f "$build/Makefile" ]; then
    (cd "$repository" && cmake --preset checker >/dev/null)
fi
(cd "$repository" && cmake --build --preset checker >/dev/null)
exec "$build/check_standards" "$@"
