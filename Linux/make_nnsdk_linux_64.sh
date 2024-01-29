#!/bin/bash
if [ ! -d "./build/" ];then
mkdir ./build
fi

cd build/
rm -rf *

cmake  -DCMAKE_TOOLCHAIN_FILE=../cmake/arm64_toolchain.cmake .. -DARM64=ON

make -j24

mkdir package
cp demo/*/aml_* package
#cp build/nnsdk* package