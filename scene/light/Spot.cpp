// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Spot.h"

namespace anari_ospray {

Spot::Spot(OSPRayGlobalState *s) : Light(s, "spot") {}

void Spot::commit()
{
  Light::commit();

  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto direction = getParam<float3>("direction", float3(0, 0, -1));
  auto openingAngle = degrees(getParam<float>("openingAngle", M_PI));
  auto falloffAngle = degrees(getParam<float>("falloffAngle", 0.1f));
  auto radius = getParam<float>("radius", 0.f);
  auto innerRadius = getParam<float>("innerRadius", 0.f);

  OSPIntensityQuantity quantity = OSP_INTENSITY_QUANTITY_INTENSITY;
  if (hasParam("intensity")) {
    intensity = getParam<float>("intensity", 1.f);
  } else if (hasParam("power")) {
    intensity = getParam<float>("power", 1.f);
    quantity = OSP_INTENSITY_QUANTITY_POWER;
  }
  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &position);
  ospSetParam(ol, "direction", OSP_VEC3F, &direction);
  ospSetParam(ol, "openingAngle", OSP_FLOAT, &openingAngle);
  ospSetParam(ol, "penumbraAngle", OSP_FLOAT, &falloffAngle);
  ospSetParam(ol, "radius", OSP_FLOAT, &radius);
  ospSetParam(ol, "innerRadius", OSP_FLOAT, &innerRadius);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &quantity);
  ospCommit(ol);
}

} // namespace anari_ospray
