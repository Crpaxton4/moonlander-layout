#!/usr/bin/env bash
set -euo pipefail

: "${APT_PACKAGES:?not set or empty}"
: "${DEB_DIR:?not set or empty}"

# Folder holds all packages that the image does not contain, so dpkg installs them
# with no package lists and no download. The runner disk is gone after the job, so
# dpkg does not sync each file to the disk
shopt -s nullglob
debs=("$DEB_DIR"/*.deb)
shopt -u nullglob
if [ "${#debs[@]}" -gt 0 ]; then
    sudo dpkg --install --force-unsafe-io "${debs[@]}"
fi

read -ra packages <<<"$APT_PACKAGES"
status=0
for pin in "${packages[@]}"; do
    name=${pin%%=*}
    version=${pin#*=}
    installed=$(dpkg-query -W -f '${Version}' "$name" 2>/dev/null || true)
    echo "$name $installed"
    if [ "$installed" != "$version" ]; then
        echo "::error::$name is '$installed', not the pinned $version"
        status=1
    fi
done
exit "$status"
