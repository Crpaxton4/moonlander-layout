#!/usr/bin/env bash
# Sets up the build environment for this layout on Ubuntu (incl. WSL).
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_HOME="$HOME/qmk_firmware"

# Installer pins. install.qmk.fm serves the bootstrap script from qmk_firmware
# master, so it is pinned to a commit instead.
QMK_BOOTSTRAP_URL="https://raw.githubusercontent.com/qmk/qmk_firmware/08c662f286ddfd12a985f57b584b02eca5af0ae6/util/env-bootstrap.sh"
FNM_VERSION="v1.39.0"

# fnm reads .node-version from the current directory
cd "$REPO_DIR"

# fnm's installer needs unzip
sudo apt-get install -y unzip

# QMK CLI and toolchain
cmp -s <(curl -fsSL "$QMK_BOOTSTRAP_URL") <(curl -fsSL https://install.qmk.fm) \
    || echo "Note: a newer QMK bootstrap script is available than the pinned one"
curl -fsSL "$QMK_BOOTSTRAP_URL" | sh -s -- --confirm
export PATH="$HOME/.local/bin:$PATH"

# Node via fnm. Its installer appends to ~/.bashrc on every run, so the shell
# setup is skipped there and added here once.
FNM_LATEST="$(curl -fsSL -o /dev/null -w '%{url_effective}' https://github.com/Schniz/fnm/releases/latest)"
[ "${FNM_LATEST##*/}" = "$FNM_VERSION" ] \
    || echo "Note: a newer fnm release (${FNM_LATEST##*/}) is available than the pinned $FNM_VERSION"
curl -fsSL "https://raw.githubusercontent.com/Schniz/fnm/$FNM_VERSION/.ci/install.sh" \
    | bash -s -- --release "$FNM_VERSION" --skip-shell
grep -qs 'fnm env' "$HOME/.bashrc" \
    || printf '\n# fnm\nexport PATH="$HOME/.local/share/fnm:$PATH"\neval "$(fnm env --shell bash)"\n' >> "$HOME/.bashrc"
export PATH="$HOME/.local/share/fnm:$PATH"
eval "$(fnm env --shell bash)"
fnm install
fnm use
npm ci --prefix "$REPO_DIR"

# ZSA's qmk_firmware fork, with QMK pointed at this repo's userspace
qmk setup zsa/qmk_firmware -b "$(cat "$REPO_DIR/src/.qmk-ref")" -H "$QMK_HOME" -y
qmk config user.overlay_dir="$REPO_DIR/src"

make -C "$REPO_DIR/src"
