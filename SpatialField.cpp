// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.hpp"

namespace anari {
namespace ospray {

SpatialField::SpatialField(std::string subtype)
    : Object(
        ospNewVolume(subtype.c_str()), subtype, CommitPriority::SPATIAL_FIELD)
{}

void SpatialField::setParam(
    const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "origin")
    id = "gridOrigin";
  if (id == "spacing")
    id = "gridSpacing";

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
