#!/usr/bin/env bash
set -euo pipefail

: "${APT_PACKAGES:?not set or empty}"
: "${DEB_DIR:?not set or empty}"

# A restored cache can contain a different file, so each run checks the versions
# before dpkg runs a package script. Script checks all packages before it fails, so
# the log shows each wrong version
read -ra packages <<<"$APT_PACKAGES"
status=0
for pin in "${packages[@]}"; do
    name=${pin%%=*}
    version=${pin#*=}
    installed=$(dpkg-query -W -f '${Version}' "$name" 2>/dev/null || true)
    shopt -s nullglob
    debs=("$DEB_DIR/${name}"_*.deb)
    shopt -u nullglob
    if [ "${#debs[@]}" -eq 0 ]; then
        echo "$name $installed in the runner image"
        actual=$installed
    else
        actual=$(dpkg-deb -f "${debs[0]}" Version || true)
        echo "$name $actual in ${debs[0]##*/}"
    fi
    if [ "$actual" != "$version" ]; then
        echo "::error::$name is '$actual', not the pinned $version"
        status=1
    fi
done
exit "$status"
