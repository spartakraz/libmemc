#!/bin/bash

set -Ee

readonly BAD_EXIT_CODE=2

err_report() {
    >&2 echo "Error on line $1"
    exit "${BAD_EXIT_CODE}"
}

trap 'err_report $LINENO' ERR 
bash -c "make -f Makefile;"

exit "${?}"
