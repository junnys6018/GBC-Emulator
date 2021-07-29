#!/bin/bash
pushd "$(dirname "$0")/.."
mkdir -p build build/Debug build/Release
cmake -S . -B build/Debug -D CMAKE_BUILD_TYPE=Debug -D GBC_USE_CXX_20=OFF
cmake -S . -B build/Release -D CMAKE_BUILD_TYPE=Release -D GBC_USE_CXX_20=OFF
popd