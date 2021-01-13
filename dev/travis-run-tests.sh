#!/usr/bin/env bash

SRC=`pwd`

rm -rf /tmp/build
mkdir -p /tmp/build
cd /tmp/build
cmake -DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER ${SRC}

make -j8 tests && ./tests/tests