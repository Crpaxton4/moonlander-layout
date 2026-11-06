#!/usr/bin/env bash
set -euo pipefail

: "${CCACHE_VERSION:?not set or empty}"
: "${CCACHE_SHA256:?not set or empty}"
: "${QMK_DISTRIB_DIR:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"

dir="$RUNNER_TEMP/ccache"
name="ccache-$CCACHE_VERSION-linux-x86_64-musl-static"
mkdir -p "$dir"
if [ ! -f "$dir/$name.tar.xz" ]; then
    curl -fsSL -o "$dir/$name.tar.xz" \
        "https://github.com/ccache/ccache/releases/download/v$CCACHE_VERSION/\
$name.tar.xz"
fi
echo "$CCACHE_SHA256  $dir/$name.tar.xz" | sha256sum -c -
bin="$RUNNER_TEMP/ccache-bin"
mkdir -p "$bin"
tar -xJf "$dir/$name.tar.xz" -C "$bin" --strip-components=1 "$name/ccache"
# Compile database must record the ccache link as the compiler for clang-tidy.
# USE_CCACHE of QMK records `ccache` instead
ln -s ccache "$bin/arm-none-eabi-gcc"
version=$("$bin/ccache" --print-version)
echo "ccache $version"
if [ "$version" != "$CCACHE_VERSION" ]; then
    echo "::error::ccache is '$version', not the pinned $CCACHE_VERSION"
    exit 1
fi
{
    echo "QMK_PATH_PREFIX=$bin"
    echo "CCACHE_PATH=$QMK_DISTRIB_DIR/bin"
    echo "CCACHE_DIR=$dir/objects"
    echo "CCACHE_COMPILERCHECK=content"
    echo "CCACHE_DEPEND=true"
    # Objects of one build use approximately 2 MB
    echo "CCACHE_MAXSIZE=50M"
} >>"$GITHUB_ENV"
CCACHE_DIR="$dir/objects" "$bin/ccache" --zero-stats
touch "$RUNNER_TEMP/ccache-build-start"
