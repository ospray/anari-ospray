// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Point.h"

namespace anari_ospray {

Point::Point(OSPRayGlobalState *s) : Light(s, "sphere") {}

void Point::commit()
{
  Light::commit();

  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto radius = getParam<float>("radius", 0.f);

  OSPIntensityQuantity quantity = OSP_INTENSITY_QUANTITY_INTENSITY;
  if (hasParam("intensity")) {
    intensity = getParam<float>("intensity", 1.f);
  } else if (hasParam("power")) {
    intensity = getParam<float>("power", 1.f);
    quantity = OSP_INTENSITY_QUANTITY_POWER;
  } else if (hasParam("radiance")) {
    intensity = getParam<float>("radiance", 1.f);
    quantity = OSP_INTENSITY_QUANTITY_RADIANCE;
  }

  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &position);
  ospSetParam(ol, "radius", OSP_FLOAT, &radius);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &quantity);
  ospCommit(ol);
}

} // namespace anari_ospray
