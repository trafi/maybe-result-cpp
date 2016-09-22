#!/usr/bin/env bash

SRC=`pwd`

rm -rf /tmp/build
mkdir -p /tmp/build
cd /tmp/build
if [ ! -d "optional" ]; then
    mkdir -p optional
    tar --strip-components=1 -xzf ${SRC}/dev/optional.tar.gz -C optional
fi
cmake -DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER -DEXPERIMENTAL_OPTIONAL_INCLUDE=/tmp/build/optional ${SRC}

make -j8 tests && ./tests/tests