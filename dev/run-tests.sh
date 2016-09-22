#!/usr/bin/env bash

SRC=`pwd`

rm -rf /tmp/build
mkdir -p /tmp/build
cd /tmp/build
if [ ! -d "optional" ]; then
    mkdir -p optional
    tar --strip-components=1 -xzf /opt/dev/optional.tar.gz -C optional
fi
cmake -DEXPERIMENTAL_OPTIONAL_INCLUDE=/tmp/build/optional ${SRC}

make -j8 tests && ./tests/tests