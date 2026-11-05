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
# Must match SHELLCHECK_VERSION in .github/workflows/lint.yaml
SHELLCHECK_VERSION="0.9.0-1"
# Must match TREE_SITTER_VERSION and TREE_SITTER_C_VERSION in
# .github/workflows/lint.yaml
TREE_SITTER_VERSION="0.20.8-2"
TREE_SITTER_C_VERSION="0.20.5-2"
# Must match DOXYGEN_VERSION and DOXYGEN_SHA256 in .github/workflows/analysis.yaml
DOXYGEN_VERSION="1.18.0"
DOXYGEN_SHA256="14fa81bdc34171edb5f1f02b1d60e74802f0439b77fa44e592565d517d72df90"

# fnm reads .node-version from the current directory
cd "$REPO_DIR"

# fnm's installer needs unzip
sudo apt-get install -y unzip

# The shellcheck pre-commit hook and actionlint both run this binary
sudo apt-get install -y "shellcheck=$SHELLCHECK_VERSION"

# The check-standards hook (tools/check_standards) builds against these
sudo apt-get install -y build-essential "libtree-sitter-dev=$TREE_SITTER_VERSION" \
    "tree-sitter-c-src=$TREE_SITTER_C_VERSION"

# QMK CLI and toolchain
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

# Node via fnm. Its installer appends to ~/.bashrc on every run, so the shell
# setup is skipped there and added here once.
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

# pre-commit runs the checks and the commit-msg hook; uv comes from QMK's
# bootstrap. `pre-commit install` refuses to run while core.hooksPath is set,
# which husky leaves behind in existing clones.
uv tool install "pre-commit==$PRE_COMMIT_VERSION"
git -C "$REPO_DIR" config --unset core.hooksPath || true
pre-commit install

# clang-tidy for `make analyze`, the same LLVM release as clang-format
uv tool install "clang-tidy==$(cat "$REPO_DIR/.llvm-version")"

# Doxygen for `make analyze`: the official release binary, the same build
# CI runs, whatever the distro packages. Only the binary leaves the tarball.
DOXYGEN_URL="https://github.com/doxygen/doxygen/releases/download/\
Release_${DOXYGEN_VERSION//./_}/doxygen-$DOXYGEN_VERSION.linux.bin.tar.gz"
DOXYGEN_TARBALL="$(mktemp)"
curl -fsSL -o "$DOXYGEN_TARBALL" "$DOXYGEN_URL"
echo "$DOXYGEN_SHA256  $DOXYGEN_TARBALL" | sha256sum -c -
mkdir -p "$HOME/.local/bin"
tar -xzf "$DOXYGEN_TARBALL" -C "$HOME/.local/bin" --strip-components=2 \
    "doxygen-$DOXYGEN_VERSION/bin/doxygen"
rm -f "$DOXYGEN_TARBALL"

# ZSA's qmk_firmware fork, with QMK pointed at this repo's userspace
qmk setup zsa/qmk_firmware -b "$(cat "$REPO_DIR/src/.qmk-ref")" -H "$QMK_HOME" -y
qmk config user.overlay_dir="$REPO_DIR/src"

make -C "$REPO_DIR"
