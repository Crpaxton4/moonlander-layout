#!/usr/bin/env bash
set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_HOME="$HOME/qmk_firmware"

# qmk_firmware master can change the bootstrap script at any time, so the pin is a
# fixed commit. An fnm release tag fixes the install script and the binary
QMK_BOOTSTRAP_COMMIT="08c662f286ddfd12a985f57b584b02eca5af0ae6"
FNM_VERSION="v1.39.0"
# src/qmk/.qmk-ref pins a commit on this branch, and CI builds from that commit
QMK_BRANCH="firmware25"
# shellcheck source=versions.env
. "$REPO_DIR/versions.env"

# `pre-commit install` uses the repository in the current directory
cd "$REPO_DIR"

# fnm installer needs unzip
sudo apt-get install -y unzip

# actionlint and the shellcheck pre-commit hook both run this binary
sudo apt-get install -y "shellcheck=$SHELLCHECK_VERSION"

# make compiledb adds the keymap.c entry with jq
sudo apt-get install -y "jq=$JQ_VERSION"

# windows CMake preset builds the key log converter with this cross compiler
sudo apt-get install -y "gcc-mingw-w64-x86-64-win32=$MINGW_VERSION"

# check-standards hook builds against these packages
sudo apt-get install -y build-essential "libtree-sitter-dev=$TREE_SITTER_VERSION" \
    "tree-sitter-c-src=$TREE_SITTER_C_VERSION"

# CMake builds the host programs and the unit tests. The unit tests use cmocka
sudo apt-get install -y "cmake=$CMAKE_VERSION" "libcmocka-dev=$CMOCKA_VERSION"

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

# fnm installer adds lines to ~/.bashrc in each run, so the script skips its
# shell setup and adds the lines one time
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
fnm install "$NODE_VERSION"
fnm use "$NODE_VERSION"
npm ci --prefix "$REPO_DIR"

# uv comes from the QMK bootstrap. `pre-commit install` does not run while
# core.hooksPath is set, and husky left that setting in older clones
uv tool install "pre-commit==$PRE_COMMIT_VERSION"
git -C "$REPO_DIR" config --unset core.hooksPath || true
pre-commit install

# clang-tidy uses the same LLVM release as clang-format
uv tool install "clang-tidy==$LLVM_VERSION"

# CI runs the same official release binary, independent of the distribution
# packages
DOXYGEN_URL="https://github.com/doxygen/doxygen/releases/download/\
Release_${DOXYGEN_VERSION//./_}/doxygen-$DOXYGEN_VERSION.linux.bin.tar.gz"
DOXYGEN_TARBALL="$(mktemp)"
curl -fsSL -o "$DOXYGEN_TARBALL" "$DOXYGEN_URL"
echo "$DOXYGEN_SHA256  $DOXYGEN_TARBALL" | sha256sum -c -
mkdir -p "$HOME/.local/bin"
tar -xzf "$DOXYGEN_TARBALL" -C "$HOME/.local/bin" --strip-components=2 \
    "doxygen-$DOXYGEN_VERSION/bin/doxygen"
rm -f "$DOXYGEN_TARBALL"

# `qmk setup` clones a branch or keeps an existing clone, so the script checks
# out the pinned commit after it
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
