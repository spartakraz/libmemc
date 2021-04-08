#!/bin/bash

set -e

bash -c "gcc -g -c -std=gnu99 -D_GNU_SOURCE=1 test.c -I. -o bin/test.o"
bash -c "gcc -L./bin ./bin/test.o -o ./bin/test -lmemc"

exit "${?}"
