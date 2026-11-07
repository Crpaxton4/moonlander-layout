#!/usr/bin/env bash
set -euo pipefail

: "${GITHUB_WORKSPACE:?not set or empty}"
: "${RUNNER_TEMP:?not set or empty}"

# Artifacts do not keep the execute permission, so the files go into a tar file.
# The Test job runs the test programs with the CTest file. The Analyze job reads
# the keymap compile database, and that database includes the headers that QMK
# writes. All paths in these files are absolute, but each job of the run has the
# same workspace. Thus, the files work there with no change
cd "$GITHUB_WORKSPACE"
tar -cf "$RUNNER_TEMP/build-output.tar" \
    .build/compile_commands.json \
    .build/cmake/CTestTestfile.cmake \
    .build/cmake/check_standards \
    .build/cmake/key_log_converter \
    .build/cmake/test/qmk/*_test \
    qmk_firmware/.build/obj_zsa_moonlander_reva_qgmlwy/src
tar -tvf "$RUNNER_TEMP/build-output.tar"
du -h "$RUNNER_TEMP/build-output.tar"
