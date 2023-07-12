// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Spot.h"

namespace anari_ospray {

Spot::Spot(OSPRayGlobalState *s) : Light(s, "spot") {}

void Spot::commit()
{
  Light::commit();

  auto color = getParam<float3>("color", float3(1, 1, 1));
  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto direction = getParam<float3>("direction", float3(0, 0, -1));
  auto openingAngle = degrees(getParam<float>("openingAngle", M_PI));
  auto irradiance = std::clamp(getParam<float>("intensity", 1.f),
      0.f,
      std::numeric_limits<float>::max());

  auto ol = osprayLight();
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &position);
  ospSetParam(ol, "direction", OSP_VEC3F, &direction);
  ospSetParam(ol, "openingAngle", OSP_FLOAT, &openingAngle);
  ospSetParam(ol, "intensity", OSP_FLOAT, &irradiance);
  ospCommit(ol);
}

} // namespace anari_ospray
