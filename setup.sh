#!/usr/bin/env bash
# Sets up the build environment for this layout on Ubuntu (incl. WSL).
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_HOME="$HOME/qmk_firmware"

# Installer pins, bumped by hand. install.qmk.fm serves QMK's bootstrap script
# from qmk_firmware master, which can change under us at any time, so the
# script is fetched from a fixed qmk_firmware commit instead (the latest commit
# that changed util/env-bootstrap.sh). fnm is pinned to a release tag, which
# fixes both its install script and the binary it downloads.
QMK_BOOTSTRAP_COMMIT="08c662f286ddfd12a985f57b584b02eca5af0ae6"
FNM_VERSION="v1.39.0"
# Must match PRE_COMMIT_VERSION in .github/workflows/lint.yaml
PRE_COMMIT_VERSION="4.7.0"

# fnm reads .node-version from the current directory
cd "$REPO_DIR"

# fnm's installer needs unzip
sudo apt-get install -y unzip

# QMK CLI and toolchain
QMK_BOOTSTRAP_LATEST="$(curl -fsSL "https://api.github.com/repos/qmk/qmk_firmware/commits?path=util/env-bootstrap.sh&per_page=1" \
    | grep -m1 -oE '"sha": "[0-9a-f]{40}"' | cut -d'"' -f4)"
[ "$QMK_BOOTSTRAP_LATEST" = "$QMK_BOOTSTRAP_COMMIT" ] \
    || echo "Note: QMK bootstrap script is pinned to $QMK_BOOTSTRAP_COMMIT; latest is $QMK_BOOTSTRAP_LATEST"
curl -fsSL "https://raw.githubusercontent.com/qmk/qmk_firmware/$QMK_BOOTSTRAP_COMMIT/util/env-bootstrap.sh" \
    | sh -s -- --confirm
export PATH="$HOME/.local/bin:$PATH"

# Node via fnm. Its installer appends to ~/.bashrc on every run, so the shell
# setup is skipped there and added here once.
FNM_LATEST="$(curl -fsSL -o /dev/null -w '%{url_effective}' https://github.com/Schniz/fnm/releases/latest)"
[ "${FNM_LATEST##*/}" = "$FNM_VERSION" ] \
    || echo "Note: fnm is pinned to $FNM_VERSION; latest is ${FNM_LATEST##*/}"
curl -fsSL "https://raw.githubusercontent.com/Schniz/fnm/$FNM_VERSION/.ci/install.sh" \
    | bash -s -- --release "$FNM_VERSION" --skip-shell
grep -qs 'fnm env' "$HOME/.bashrc" \
    || printf '\n# fnm\nexport PATH="$HOME/.local/share/fnm:$PATH"\neval "$(fnm env --shell bash)"\n' >> "$HOME/.bashrc"
export PATH="$HOME/.local/share/fnm:$PATH"
eval "$(fnm env --shell bash)"
fnm install
fnm use
npm ci --prefix "$REPO_DIR"

# pre-commit runs the checks and the commit-msg hook; uv comes from QMK's
# bootstrap. `pre-commit install` refuses to run while core.hooksPath is set,
# which husky leaves behind in existing clones.
uv tool install "pre-commit==$PRE_COMMIT_VERSION"
git -C "$REPO_DIR" config --unset core.hooksPath || true
pre-commit install

# ZSA's qmk_firmware fork, with QMK pointed at this repo's userspace
qmk setup zsa/qmk_firmware -b "$(cat "$REPO_DIR/src/.qmk-ref")" -H "$QMK_HOME" -y
qmk config user.overlay_dir="$REPO_DIR/src"

make -C "$REPO_DIR/src"
