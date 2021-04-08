#!/bin/bash

set -e

valgrind --tool=memcheck --leak-check=yes ./bin/test

exit "${?}"
