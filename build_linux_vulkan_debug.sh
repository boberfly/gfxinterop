#!/usr/bin/env bash

set echo off

cmake -E make_directory build/linux_debug
cd build/linux_debug
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../install/linux_debug $@ ../..
make -j `getconf _NPROCESSORS_ONLN`
make install

set echo on
