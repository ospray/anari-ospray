// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Spot.h"

namespace anari_ospray {

Spot::Spot(OSPRayGlobalState *s) : Light(s, "spot") {}

void Spot::commitParameters()
{
  Light::commitParameters();

  m_position = getParam<float3>("position", float3(0, 0, 0));
  m_direction = getParam<float3>("direction", float3(0, 0, -1));
  m_openingAngle = degrees(getParam<float>("openingAngle", M_PI));
  m_falloffAngle = degrees(getParam<float>("falloffAngle", 0.1f));
  m_radius = getParam<float>("radius", 0.f);
  m_innerRadius = getParam<float>("innerRadius", 0.f);

  m_quantity = OSP_INTENSITY_QUANTITY_INTENSITY;
  if (hasParam("intensity")) {
    intensity = getParam<float>("intensity", 1.f);
  } else if (hasParam("power")) {
    intensity = getParam<float>("power", 1.f);
    m_quantity = OSP_INTENSITY_QUANTITY_POWER;
  }
}

void Spot::finalize()
{
  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &m_position);
  ospSetParam(ol, "direction", OSP_VEC3F, &m_direction);
  ospSetParam(ol, "openingAngle", OSP_FLOAT, &m_openingAngle);
  ospSetParam(ol, "penumbraAngle", OSP_FLOAT, &m_falloffAngle);
  ospSetParam(ol, "radius", OSP_FLOAT, &m_radius);
  ospSetParam(ol, "innerRadius", OSP_FLOAT, &m_innerRadius);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &m_quantity);
  ospCommit(ol);
}

} // namespace anari_ospray
