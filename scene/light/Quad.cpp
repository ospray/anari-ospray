// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Quad.h"

namespace anari_ospray {

QuadLight::QuadLight(OSPRayGlobalState *s) : Light(s, "quad") {}

void QuadLight::commit()
{
  Light::commit();

  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto edge1 = getParam<float3>("edge1", float3(1, 0, 0));
  auto edge2 = getParam<float3>("edge2", float3(0, 1, 0));
  if (getParamString("side", "front") == "back")
    std::swap(edge1, edge2);

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
  ospSetParam(ol, "edge1", OSP_VEC3F, &edge1);
  ospSetParam(ol, "edge2", OSP_VEC3F, &edge2);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospSetParam(ol, "intensityQuantity", OSP_UINT, &quantity);
  ospCommit(ol);
}

} // namespace anari_ospray
