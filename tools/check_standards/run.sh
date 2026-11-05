#!/bin/sh
# Builds the standards checker if needed, then checks the files given
make -s -C "${0%/*}/../.." checker && exec "${0%/*}/build/check_standards" "$@"
