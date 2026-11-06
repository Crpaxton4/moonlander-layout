#!/bin/sh
make -s -C "${0%/*}/../.." checker \
    && exec "${0%/*}/../../.build/check_standards/check_standards" "$@"
