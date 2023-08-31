#!/bin/bash -x
## Copyright 2023 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

export LD_LIBRARY_PATH=./build/install/lib:./build/install/lib64:$LD_LIBRARY_PATH
export ANARI_LIBRARY=ospray

./build/install/bin/anariRenderTests
