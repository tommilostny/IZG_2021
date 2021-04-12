#!/bin/sh

mkdir build
cd build
cmake -D CMAKE_BUILD_TYPE=DEBUG ..
make
