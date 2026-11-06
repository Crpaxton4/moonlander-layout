#!/usr/bin/env bash
# This script installs the build environment for this layout on Ubuntu, also in
# WSL.
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_HOME="$HOME/qmk_firmware"

# The installer pins. Change them by hand. The official QMK install site gives the
# QMK bootstrap script from qmk_firmware master, which can change at any time.
# Thus the script comes from a fixed qmk_firmware commit: the last commit that
# changed util/env-bootstrap.sh. The fnm pin is a release tag, which fixes the
# install script and the binary that it downloads.
QMK_BOOTSTRAP_COMMIT="08c662f286ddfd12a985f57b584b02eca5af0ae6"
FNM_VERSION="v1.39.0"
# The qmk_firmware branch of ZSA for the Moonlander. src/qmk/.qmk-ref pins a
# commit on this branch. CI builds from the same commit.
QMK_BRANCH="firmware25"
# It must be the same as PRE_COMMIT_VERSION in .github/workflows/lint.yaml.
PRE_COMMIT_VERSION="4.7.0"
# It must be the same as SHELLCHECK_VERSION in .github/workflows/lint.yaml.
SHELLCHECK_VERSION="0.9.0-1"
# These must be the same as TREE_SITTER_VERSION and TREE_SITTER_C_VERSION in
# .github/workflows/lint.yaml.
TREE_SITTER_VERSION="0.20.8-2"
TREE_SITTER_C_VERSION="0.20.5-2"
# These must be the same as DOXYGEN_VERSION and DOXYGEN_SHA256 in
# .github/workflows/build.yaml.
DOXYGEN_VERSION="1.18.0"
DOXYGEN_SHA256="14fa81bdc34171edb5f1f02b1d60e74802f0439b77fa44e592565d517d72df90"

# fnm reads .node-version from the current directory.
cd "$REPO_DIR"

# The fnm installer needs unzip.
sudo apt-get install -y unzip

# The shellcheck pre-commit hook and actionlint both run this binary.
sudo apt-get install -y "shellcheck=$SHELLCHECK_VERSION"

# The check-standards hook (src/check_standards) builds against these packages.
sudo apt-get install -y build-essential "libtree-sitter-dev=$TREE_SITTER_VERSION" \
    "tree-sitter-c-src=$TREE_SITTER_C_VERSION"

# The QMK CLI and toolchain.
QMK_COMMITS_URL="https://api.github.com/repos/qmk/qmk_firmware/commits\
?path=util/env-bootstrap.sh&per_page=1"
QMK_BOOTSTRAP_URL="https://raw.githubusercontent.com/qmk/qmk_firmware/\
$QMK_BOOTSTRAP_COMMIT/util/env-bootstrap.sh"
QMK_BOOTSTRAP_LATEST="$(curl -fsSL "$QMK_COMMITS_URL" \
    | grep -m1 -oE '"sha": "[0-9a-f]{40}"' | cut -d'"' -f4)"
[ "$QMK_BOOTSTRAP_LATEST" = "$QMK_BOOTSTRAP_COMMIT" ] \
    || echo "Note: QMK bootstrap script is pinned to $QMK_BOOTSTRAP_COMMIT;" \
        "latest is $QMK_BOOTSTRAP_LATEST"
curl -fsSL "$QMK_BOOTSTRAP_URL" \
    | sh -s -- --confirm
export PATH="$HOME/.local/bin:$PATH"

# Node through fnm. The fnm installer adds lines to ~/.bashrc in each run. Thus
# the script skips the shell setup of the installer and adds it here one time.
FNM_RELEASES_URL="https://github.com/Schniz/fnm/releases/latest"
FNM_LATEST="$(curl -fsSL -o /dev/null -w '%{url_effective}' "$FNM_RELEASES_URL")"
[ "${FNM_LATEST##*/}" = "$FNM_VERSION" ] \
    || echo "Note: fnm is pinned to $FNM_VERSION; latest is ${FNM_LATEST##*/}"
curl -fsSL "https://raw.githubusercontent.com/Schniz/fnm/$FNM_VERSION/.ci/install.sh" \
    | bash -s -- --release "$FNM_VERSION" --skip-shell
grep -qs 'fnm env' "$HOME/.bashrc" \
    || cat >>"$HOME/.bashrc" <<'EOF'

# fnm
export PATH="$HOME/.local/share/fnm:$PATH"
eval "$(fnm env --shell bash)"
EOF
export PATH="$HOME/.local/share/fnm:$PATH"
eval "$(fnm env --shell bash)"
fnm install
fnm use
npm ci --prefix "$REPO_DIR"

# pre-commit runs the checks and the commit-msg hook. uv comes from the QMK
# bootstrap. `pre-commit install` does not run while core.hooksPath has a value,
# and husky left this setting in older clones.
uv tool install "pre-commit==$PRE_COMMIT_VERSION"
git -C "$REPO_DIR" config --unset core.hooksPath || true
pre-commit install

# clang-tidy for `make analyze`, at the same LLVM release as clang-format.
uv tool install "clang-tidy==$(cat "$REPO_DIR/.llvm-version")"

# Doxygen for `make analyze`: the official release binary. CI runs the same
# build, independent of the distribution packages. The script extracts only the
# binary from the tarball.
DOXYGEN_URL="https://github.com/doxygen/doxygen/releases/download/\
Release_${DOXYGEN_VERSION//./_}/doxygen-$DOXYGEN_VERSION.linux.bin.tar.gz"
DOXYGEN_TARBALL="$(mktemp)"
curl -fsSL -o "$DOXYGEN_TARBALL" "$DOXYGEN_URL"
echo "$DOXYGEN_SHA256  $DOXYGEN_TARBALL" | sha256sum -c -
mkdir -p "$HOME/.local/bin"
tar -xzf "$DOXYGEN_TARBALL" -C "$HOME/.local/bin" --strip-components=2 \
    "doxygen-$DOXYGEN_VERSION/bin/doxygen"
rm -f "$DOXYGEN_TARBALL"

# The qmk_firmware fork of ZSA at the pinned commit, with QMK pointed to the
# userspace of this repository. `qmk setup` clones a branch, or keeps an existing
# clone without change. Thus the script checks out the pinned commit and its
# submodules after `qmk setup`.
QMK_URL="https://github.com/zsa/qmk_firmware"
QMK_COMMIT="$(cat "$REPO_DIR/src/qmk/.qmk-ref")"
QMK_LATEST="$(git ls-remote "$QMK_URL" "refs/heads/$QMK_BRANCH" | cut -f1)"
[ "$QMK_LATEST" = "$QMK_COMMIT" ] \
    || echo "Note: ZSA qmk_firmware is pinned to $QMK_COMMIT;" \
        "latest $QMK_BRANCH is $QMK_LATEST"
qmk setup zsa/qmk_firmware -b "$QMK_BRANCH" -H "$QMK_HOME" -y
git -C "$QMK_HOME" cat-file -e "$QMK_COMMIT^{commit}" 2>/dev/null \
    || git -C "$QMK_HOME" fetch "$QMK_URL" "$QMK_COMMIT"
git -C "$QMK_HOME" checkout --detach "$QMK_COMMIT"
git -C "$QMK_HOME" submodule update --init --recursive
qmk config user.overlay_dir="$REPO_DIR/src/qmk"

make -C "$REPO_DIR"
