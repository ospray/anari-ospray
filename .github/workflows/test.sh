#!/bin/bash -x
## Copyright 2023 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

export LD_LIBRARY_PATH=./build/install/lib:./build/install/lib64:$LD_LIBRARY_PATH
export ANARI_LIBRARY=ospray

for s in \
  instanced_cubes\
  pbr_spheres\
  random_cylinders\
  random_spheres\
  textured_cube\
  triangle_attributes\
; do
  ./build/install/bin/anariRenderTests --scene test $s
done

for s in \
  cornell_box\
  gravity_spheres_volume\
; do
  ./build/install/bin/anariRenderTests --scene demo $s
done
