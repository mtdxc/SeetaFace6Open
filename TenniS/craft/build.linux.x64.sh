#!/usr/bin/env bash

export BUILD_DIR=build.linux.x64
export BUILD_TYPE=Release

export INSTALL_DIR=$(cd "$(dirname "$0")"; pwd)/../../build

HOME=$(cd `dirname $0`; pwd)

cd $HOME

mkdir "$BUILD_DIR"

cd "$BUILD_DIR"


cmake "$HOME/.." \
-DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
-DCONFIGURATION="$BUILD_TYPE" \
-DCMAKE_INSTALL_PREFIX="$INSTALL_DIR" \
-DTS_USE_OPENMP=ON \
-DTS_USE_SIMD=ON \
-DTS_ON_HASWELL=ON \
-DTS_DYNAMIC_INSTRUCTION=ON

make -j16

make install
