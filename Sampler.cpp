// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.hpp"

namespace anari {
namespace ospray {

Sampler::Sampler(std::string subtype)
    : Object(ospNewTexture(subtype.c_str()), subtype)
{}

void Sampler::commit()
{
  int fmt = OSP_TEXTURE_RGB32F;
  ospSetParam(m_object, "format", OSP_INT, &fmt);

  Object::commit();
}

void Sampler::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "image")
    id = "data";

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
