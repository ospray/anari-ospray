// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Directional.h"

namespace anari_ospray {

Directional::Directional(OSPRayGlobalState *s) : Light(s, "distant") {}

void Directional::commitParameters()
{
  Light::commitParameters();

  m_direction = getParam<float3>("direction", float3(0, 0, -1));
  m_angularDiameter = degrees(getParam<float>("angularDiameter", 0.f));

  m_quantity = OSP_INTENSITY_QUANTITY_IRRADIANCE;
  if (hasParam("irradiance")) {
    intensity = getParam<float>("irradiance", 1.f);
  } else if (hasParam("radiance")) {
    intensity = getParam<float>("radiance", 1.f);
    m_quantity = OSP_INTENSITY_QUANTITY_RADIANCE;
  }
}

void Directional::finalize()
{
  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "direction", OSP_VEC3F, &m_direction);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &m_quantity);
  ospSetParam(ol, "angularDiameter", OSP_FLOAT, &m_angularDiameter);
  ospCommit(ol);
}

} // namespace anari_ospray
