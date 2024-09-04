## Copyright 2024 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

anari_generate_queries(
  NAME ospray
  PREFIX OSPRayDevice
  CPP_NAMESPACE anari_ospray
  JSON_DEFINITIONS_FILE ${CMAKE_CURRENT_SOURCE_DIR}/ospray_device.json
  JSON_ROOT_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}
)

add_custom_target(
  generate_queries
  COMMAND clang-format -style=file -i OSPRayDeviceQueries.cpp
  DEPENDS generate_ospray
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
