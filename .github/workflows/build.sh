#!/bin/bash -x
## Copyright 2020-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

mkdir build
cd build

cmake --version
cmake -DCMAKE_INSTALL_PREFIX=install ../superbuild
cmake --build .
