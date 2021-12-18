// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Surface.hpp"

namespace anari {
namespace ospray {

Surface::Surface()
    : Object(ospNewGeometricModel(), "<none>", CommitPriority::SURFACE)
{}

void Surface::commit()
{
  if (!m_geometry)
    throw std::runtime_error("no Geometry given to Surface!");

  for (auto &p : m_geometry->m_modelParams)
    Object::setParam(p.id(), p.type(), p.mem());

  Object::commit();
}

void Surface::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "geometry")
    m_geometry = *(Geometry **)mem;

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
