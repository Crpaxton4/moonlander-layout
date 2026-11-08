#!/usr/bin/env bash
set -euo pipefail

: "${CCACHE_VERSION:?not set or empty}"
: "${CCACHE_SHA256:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"
: "${GITHUB_ENV:?not set or empty}"

# Release file goes into the cache folder, so a restored cache contains it
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
version=$("$bin/ccache" --print-version)
echo "ccache $version"
if [ "$version" != "$CCACHE_VERSION" ]; then
    echo "::error::ccache is '$version', not the pinned $CCACHE_VERSION"
    exit 1
fi
# Each CMake configure step of the job uses ccache as the compiler launcher. CMake
# does not write the launcher into the compile databases
{
    echo "CMAKE_C_COMPILER_LAUNCHER=$bin/ccache"
    echo "CCACHE_DIR=$dir/objects"
    echo "CCACHE_COMPILERCHECK=content"
    echo "CCACHE_DEPEND=true"
    # Objects of one build use less than 10 MB
    echo "CCACHE_MAXSIZE=50M"
} | tee -a "$GITHUB_ENV"
CCACHE_DIR="$dir/objects" "$bin/ccache" --zero-stats
touch "$RUNNER_TEMP/ccache-build-start"
