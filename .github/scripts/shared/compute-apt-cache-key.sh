#!/usr/bin/env bash
set -euo pipefail

: "${APT_PACKAGES:?not set or empty}"
: "${RUNNER_OS:?not set or empty}"
: "${RUNNER_ARCH:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${ImageVersion:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

# apt downloads only the dependencies that the runner image does not contain, so the
# key contains the image. Each pin is in the hash, so a new pin never restores an old
# package
read -ra packages <<<"$APT_PACKAGES"
pins_hash=$(printf '%s\n' "${packages[@]}" | sort | sha256sum | cut -c1-16)
key="apt-$RUNNER_OS-$RUNNER_ARCH-$ImageOS-$ImageVersion"
key="$key-${packages[0]%%=*}-$pins_hash"
echo "key=$key" | tee -a "$GITHUB_OUTPUT"
