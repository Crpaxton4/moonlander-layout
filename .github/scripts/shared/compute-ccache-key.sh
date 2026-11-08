#!/usr/bin/env bash
set -euo pipefail

: "${CCACHE_NAME:?not set or empty}"
: "${COMPILER:?not set or empty}"
: "${FLAGS_HASH:?not set or empty}"
# Empty is valid: only a build with a compiler plugin adds the plugin release
: "${KEY_EXTRA?not set}"
: "${CCACHE_VERSION:?not set or empty}"
: "${RUNNER_OS:?not set or empty}"
: "${ImageOS:?not set or empty}"
: "${GITHUB_RUN_ID:?not set or empty}"
: "${GITHUB_RUN_ATTEMPT:?not set or empty}"
: "${GITHUB_OUTPUT:?not set or empty}"

# clang has no -dumpfullversion. The hash of the version text also changes for a
# new package build of the same compiler release
"$COMPILER" --version
version=$("$COMPILER" -dumpfullversion 2>/dev/null || "$COMPILER" -dumpversion)
version_hash=$("$COMPILER" --version | sha256sum | cut -c1-8)
# ccache hashes the compiler and all flags, so an object of other flags gives only a
# miss. Thus the compiler prefix can also restore an entry of other flags, and the
# objects of the files with no change stay hits. A compiler or plugin change gives a
# different compiler prefix, so it never restores an old entry
compiler_prefix="ccache-$CCACHE_NAME-$RUNNER_OS-$ImageOS-$CCACHE_VERSION"
compiler_prefix="$compiler_prefix-${COMPILER##*/}-$version-$version_hash"
if [ -n "$KEY_EXTRA" ]; then
    compiler_prefix="$compiler_prefix-$KEY_EXTRA"
fi
compiler_prefix="$compiler_prefix-"
prefix="${compiler_prefix}flags-${FLAGS_HASH:0:16}-"
{
    echo "compiler-prefix=$compiler_prefix"
    echo "prefix=$prefix"
    echo "key=$prefix$GITHUB_RUN_ID-$GITHUB_RUN_ATTEMPT"
} | tee -a "$GITHUB_OUTPUT"
