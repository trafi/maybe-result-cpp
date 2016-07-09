#!/usr/bin/env bash

SRC=`pwd`

rm -rf /tmp/build
mkdir -p /tmp/build
cd /tmp/build
if [ ! -d "optional" ]; then
    mkdir -p optional
    curl -Ls https://api.github.com/repos/akrzemi1/Optional/tarball | tar --strip-components=1 -xz -C optional
fi
cmake -DCMAKE_CXX_COMPILER=$CXX_COMPILER -DCMAKE_C_COMPILER=$C_COMPILER -DEXPERIMENTAL_OPTIONAL_INCLUDE=/tmp/build/optional ${SRC}

make -j8 tests && ./tests/tests