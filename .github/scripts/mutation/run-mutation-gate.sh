#!/usr/bin/env bash
set -euo pipefail

# A PR checkout is a merge commit with the base branch as its first parent, so the
# diff to HEAD^1 contains only the changes of the PR
cmake --preset mutation-diff "-DMUTATION_DIFF_REF=HEAD^1"
time cmake --build --preset mutation-diff
