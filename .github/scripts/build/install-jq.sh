#!/usr/bin/env bash
set -euo pipefail

: "${JQ_VERSION:?not set or empty}"

# Runner images ship a jq that can change between image releases
sudo apt-get update
sudo apt-get install -y --allow-downgrades "jq=$JQ_VERSION"
jq --version
