## Copyright 2021 Intel Corporation
## Copyright 2021 The Khronos Group
## SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.11)

## Project setup ##

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_C_STANDARD   99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the build type." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo")
endif()

project(anari_library_ospray VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

include(GNUInstallDirs)

## Dependencies ##
find_package(ospray 2.10.0 REQUIRED)
find_package(anari 0.3.0 EXACT REQUIRED)
find_package(Python3 REQUIRED COMPONENTS Interpreter)

add_custom_command(
  OUTPUT
    "${CMAKE_CURRENT_BINARY_DIR}/OSPRayQueries.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/OSPRayQueries.h"
  COMMAND
    ${Python3_EXECUTABLE} ${ANARI_CODE_GEN_ROOT}/generate_queries.py
      --device "${CMAKE_CURRENT_SOURCE_DIR}/api/anari_ospray_device.json"
      --json "${ANARI_CODE_GEN_ROOT}/api"
      --json "${CMAKE_CURRENT_SOURCE_DIR}/api"
      --namespace anari::ospray
      --output "${CMAKE_CURRENT_BINARY_DIR}"
      --prefix OSPRay
      --includefile
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/api/anari_ospray_device.json"
    "${CMAKE_CURRENT_SOURCE_DIR}/api/anari_ospray_objects.json"
)

add_library(${PROJECT_NAME} SHARED
  OSPRayDevice.cpp

  Object.cpp
  Array.cpp
  Camera.cpp
  Frame.cpp
  Geometry.cpp
  Group.cpp
  Instance.cpp
  Light.cpp
  Material.cpp
  Renderer.cpp
  Sampler.cpp
  SpatialField.cpp
  Surface.cpp
  Volume.cpp
  World.cpp

  util/ParamWrapper.cpp

  "${CMAKE_CURRENT_BINARY_DIR}/OSPRayQueries.cpp"
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    "${CMAKE_CURRENT_BINARY_DIR}"
)


target_link_libraries(${PROJECT_NAME}
PUBLIC
  ospray::ospray
  anari::anari
  anari::anari_utilities
)

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
