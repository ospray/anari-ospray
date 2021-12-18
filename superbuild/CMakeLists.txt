## Copyright 2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.11)
project(anari-ospray-superbuild)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")
endif()

include(ExternalProject)

ExternalProject_Add(ospray
  URL https://github.com/ospray/ospray/archive/devel.zip
  SOURCE_SUBDIR scripts/superbuild
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    -DINSTALL_IN_SEPARATE_DIRECTORIES=OFF
    -DBUILD_EMBREE_FROM_SOURCE=OFF
    -DBUILD_GLFW=OFF
    -DBUILD_OSPRAY_APPS=OFF
  INSTALL_COMMAND ""
)

ExternalProject_Add(anari
  URL https://github.com/KhronosGroup/ANARI-SDK/archive/main.zip
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

ExternalProject_Add(anari-ospray
  SOURCE_SUBDIR ../../../../
  DOWNLOAD_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
string(REPLACE ";" "|" CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")

ExternalProject_Add_StepDependencies(anari-ospray configure ospray anari)
