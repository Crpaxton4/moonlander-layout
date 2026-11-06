#!/usr/bin/env bash
set -euo pipefail

: "${DEB_DIR:?not set or empty}"
: "${SHELLCHECK_VERSION:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_PATH:?not set or empty}"

actual=$(dpkg-deb -f "$DEB_DIR"/shellcheck_*.deb Version || true)
echo "shellcheck $actual"
if [ "$actual" != "$SHELLCHECK_VERSION" ]; then
    echo "::error::shellcheck is '$actual', not the pinned $SHELLCHECK_VERSION"
    exit 1
fi
root="$RUNNER_TEMP/shellcheck-root"
dpkg-deb -x "$DEB_DIR"/shellcheck_*.deb "$root"
# GITHUB_PATH puts the extracted shellcheck before the shellcheck of the runner
# image
echo "$root/usr/bin" >>"$GITHUB_PATH"
