#!/bin/sh
# The entry of the check-standards hook. It builds the checker if necessary. Then
# it checks the given files.
make -s -C "${0%/*}/../.." checker \
    && exec "${0%/*}/../../.build/check_standards/check_standards" "$@"
