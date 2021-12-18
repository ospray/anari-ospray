// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.hpp"

namespace anari {
namespace ospray {

Geometry::Geometry(std::string subtype)
    : Object(ospNewGeometry(subtype.c_str()), subtype, CommitPriority::GEOMETRY)
{}

void Geometry::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "primitive.index")
    id = "index";

  if (m_subtype == "sphere") {
    if (id == "vertex.position")
      id = "sphere.position";
    if (id == "vertex.radius")
      id = "sphere.radius";
    if (id == "vertex.attribute0")
      id = "sphere.texcoord";
    if (id == "vertex.color") {
      m_modelParams.emplace_back("color", type, mem);
      return;
    }
  }

  Object::setParam(id.c_str(), type, mem);
}
} // namespace ospray
} // namespace anari
