#!/usr/bin/env bash

COMMAND="./dev/run-tests.sh"

compilers=( "clang-3.7" "clang-3.8" "gcc-4.9" "gcc-5.2" "gcc-6.1" )

for compiler in "${compilers[@]}"
do

    echo "Env ${compiler}"

    docker run -v `pwd`:/opt -w=/opt nercury/cmake-cpp:${compiler} /bin/bash -c "${COMMAND}"
    if [ $? -ne 0 ]; then
        echo "Tests failed on ${compiler}"
        exit 1
    fi

done

echo "OK!"