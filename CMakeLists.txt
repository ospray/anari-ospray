## Copyright 2022 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.11)

message(STATUS "CMake version: ${CMAKE_VERSION}")

## Language setup ##

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_BUILD_TYPE_INIT Release)

set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_INSTALL_MESSAGE LAZY)

## Establish project ##

project(anari_ospray)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include(GNUInstallDirs)
include(cmake/cmake_project_commands.cmake)

## Dependencies ##

find_package(anari REQUIRED)
find_package(ospray REQUIRED)

project(anari_library_ospray LANGUAGES CXX)

## Code generation ##

anari_generate_queries(
  NAME ospray
  PREFIX OSPRayDevice
  CPP_NAMESPACE anari_ospray
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ospray_extensions.json
  JSON_ROOT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
)

anari_generate_debug_objects(
  NAME ospray
  PREFIX OSPRayDevice
  CPP_NAMESPACE anari_ospray
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ospray_extensions.json
  JSON_ROOT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
)

## Core device target ##

project_add_library(SHARED)

project_sources(PRIVATE
  OSPRayDevice.cpp
  OSPRayDeviceQueries.cpp
  OSPRayDeviceDebugFactory.cpp
  OSPRayGlobalState.cpp
  OSPRayLibrary.cpp
  Object.cpp
  array/Array.cpp
  array/Array1D.cpp
  array/Array2D.cpp
  array/Array3D.cpp
  array/ObjectArray.cpp
  camera/Camera.cpp
  camera/Orthographic.cpp
  camera/Perspective.cpp
  frame/Frame.cpp
  renderer/Debug.cpp
  renderer/Pathtracer.cpp
  renderer/Renderer.cpp
  renderer/SciVis.cpp
  scene/Group.cpp
  scene/Instance.cpp
  scene/World.cpp
  scene/light/Directional.cpp
  scene/light/Light.cpp
  scene/light/Point.cpp
  scene/light/Spot.cpp
  scene/surface/Surface.cpp
  scene/surface/geometry/Cone.cpp
  scene/surface/geometry/Curve.cpp
  scene/surface/geometry/Cylinder.cpp
  scene/surface/geometry/Geometry.cpp
  scene/surface/geometry/Quad.cpp
  scene/surface/geometry/Sphere.cpp
  scene/surface/geometry/Triangle.cpp
  scene/surface/material/Material.cpp
  scene/surface/material/Matte.cpp
  scene/surface/material/PBM.cpp
  scene/surface/material/sampler/Image1D.cpp
  scene/surface/material/sampler/Image2D.cpp
  scene/surface/material/sampler/Sampler.cpp
  scene/volume/TransferFunction1D.cpp
  scene/volume/Volume.cpp
  scene/volume/spatial_field/AMRField.cpp
  scene/volume/spatial_field/SpatialField.cpp
  scene/volume/spatial_field/StructuredRegularField.cpp
)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME}
  EXPORT_MACRO_NAME "ANARI_OSPRAY_DEVICE_INTERFACE"
)

project_include_directories(
PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

project_link_libraries(
PUBLIC
  anari::anari_library_debug
  anari::helium
  ospray::ospray
)

if(WIN32)
  project_compile_definitions(PRIVATE _USE_MATH_DEFINES)
endif()

## Installation ##

install(TARGETS ${PROJECT_NAME}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
FILES
  ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
  ${CMAKE_CURRENT_LIST_DIR}/include/anari/ext/ospray/anariNewOSPRayDevice.h
DESTINATION
  ${CMAKE_INSTALL_INCLUDEDIR}/anari/ext/ospray
)
