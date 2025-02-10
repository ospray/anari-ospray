// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Point.h"

namespace anari_ospray {

Point::Point(OSPRayGlobalState *s) : Light(s, "sphere") {}

void Point::commitParameters()
{
  Light::commitParameters();
  m_position = getParam<float3>("position", float3(0, 0, 0));
  m_radius = getParam<float>("radius", 0.f);
  m_quantity = OSP_INTENSITY_QUANTITY_INTENSITY;
  if (hasParam("intensity")) {
    intensity = getParam<float>("intensity", 1.f);
  } else if (hasParam("power")) {
    intensity = getParam<float>("power", 1.f);
    m_quantity = OSP_INTENSITY_QUANTITY_POWER;
  } else if (hasParam("radiance")) {
    intensity = getParam<float>("radiance", 1.f);
    m_quantity = OSP_INTENSITY_QUANTITY_RADIANCE;
  }
}

void Point::finalize()
{
  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &m_position);
  ospSetParam(ol, "radius", OSP_FLOAT, &m_radius);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &m_quantity);
  ospCommit(ol);
}

} // namespace anari_ospray
