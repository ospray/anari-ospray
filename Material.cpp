// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Material.hpp"

namespace anari {
namespace ospray {

Material::Material(std::string subtype)
    : Object(ospNewMaterial(NULL, subtype.c_str()), subtype)
{}

void Material::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);
  if (id == "color")
    id = "kd";
  if (id == "opacity")
    id = "d";

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
