#! /usr/bin/env bash

set -e

make install-deps
make test-jest
make coverage
echo "Ok."
