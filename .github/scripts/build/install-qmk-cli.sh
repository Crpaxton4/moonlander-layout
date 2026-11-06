#!/usr/bin/env bash
set -euo pipefail

: "${QMK_PYTHON_DEPS_DATE:?not set or empty}"
: "${QMK_CLI_VERSION:?not set or empty}"

# Python code of qmk_firmware runs in the QMK CLI environment
uv tool install --exclude-newer "$QMK_PYTHON_DEPS_DATE" \
    --with-requirements qmk_firmware/requirements.txt \
    "qmk==$QMK_CLI_VERSION"
qmk --version
"$(uv tool dir)/qmk/bin/python" --version
