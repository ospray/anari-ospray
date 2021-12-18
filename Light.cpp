// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Light.hpp"

namespace anari {
namespace ospray {

Light::Light(std::string subtype)
    : Object(ospNewLight(subtype.c_str()), subtype)
{
  if (subtype == "distant" || subtype == "spot" || subtype == "hdri") {
    float d[3]{0, 0, -1};
    ospSetParam(m_object, "direction", OSP_VEC3F, d); // set default direction
  }
}

void Light::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "irradiance") {
    id = "intensity";
    uint8_t iq = OSP_INTENSITY_QUANTITY_IRRADIANCE;
    Object::setParam("intensityQuantity", ANARI_UINT8, &iq);
  } else if (id == "radiance") {
    id = "intensity";
    uint8_t iq = OSP_INTENSITY_QUANTITY_RADIANCE;
    Object::setParam("intensityQuantity", ANARI_UINT8, &iq);
  }

  Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
