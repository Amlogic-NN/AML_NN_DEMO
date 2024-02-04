#!/bin/bash
if [ ! -d "./build/" ];then
mkdir ./build
fi

cd build/
rm -rf *

cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/arm32_toolchain.cmake .. -DVSI=ON -DARM32=ON

make -j24

mkdir package
cp demo/vsi/*/*/aml_*_vsi_32 package
