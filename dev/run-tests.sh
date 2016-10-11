#!/usr/bin/env bash

SRC=`pwd`

rm -rf /tmp/build
mkdir -p /tmp/build
cd /tmp/build
cmake ${SRC}

make -j8 tests && ./tests/tests