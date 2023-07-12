// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Point.h"

namespace anari_ospray {

Point::Point(OSPRayGlobalState *s) : Light(s, "sphere") {}

void Point::commit()
{
  Light::commit();

  auto color = getParam<float3>("color", float3(1, 1, 1));
  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto radius = getParam<float>("radius", 0.f);
  auto irradiance = std::clamp(getParam<float>("intensity", 1.f),
      0.f,
      std::numeric_limits<float>::max());

  auto ol = osprayLight();
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "position", OSP_VEC3F, &position);
  ospSetParam(ol, "radius", OSP_FLOAT, &radius);
  ospSetParam(ol, "intensity", OSP_FLOAT, &irradiance);
  ospCommit(ol);
}

} // namespace anari_ospray
