// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Directional.h"

namespace anari_ospray {

Directional::Directional(OSPRayGlobalState *s) : Light(s, "distant") {}

void Directional::commit()
{
  Light::commit();

  auto color = getParam<float3>("color", float3(1, 1, 1));
  auto direction = getParam<float3>("direction", float3(0, 0, -1));
  auto irradiance = std::clamp(getParam<float>("irradiance", 1.f),
      0.f,
      std::numeric_limits<float>::max());

  auto ol = osprayLight();
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "direction", OSP_VEC3F, &direction);
  ospSetParam(ol, "intensity", OSP_FLOAT, &irradiance);
  ospCommit(ol);
}

} // namespace anari_ospray
