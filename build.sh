#!/bin/bash

mkdir -p build
cd build
cmake ..
make
cd ..
cp build/cpp-df CppDf