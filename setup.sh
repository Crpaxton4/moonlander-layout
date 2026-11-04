#!/usr/bin/env bash
# Sets up the build environment for this layout on Ubuntu (incl. WSL).
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_HOME="$HOME/qmk_firmware"

# fnm's installer needs unzip
sudo apt-get install -y unzip

curl -fsSL https://install.qmk.fm | sh -s -- --confirm
export PATH="$HOME/.local/bin:$PATH"

curl -fsSL https://fnm.vercel.app/install | bash
export PATH="$HOME/.local/share/fnm:$PATH"
eval "$(fnm env --shell bash)"
fnm install --lts
npm install --prefix "$REPO_DIR"

qmk setup zsa/qmk_firmware -b "$(cat "$REPO_DIR/.qmk-ref")" -H "$QMK_HOME" -y
qmk config user.overlay_dir="$REPO_DIR"

make -C "$REPO_DIR"
