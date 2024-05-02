// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Quad.h"

namespace anari_ospray {

QuadLight::QuadLight(OSPRayGlobalState *s) : Light(s, "quad") {}

void QuadLight::commit()
{
  Light::commit();

  auto color = getParam<float3>("color", float3(1, 1, 1));
  auto visible = getParam<bool>("visible", true);
  auto position = getParam<float3>("position", float3(0, 0, 0));
  auto edge1 = getParam<float3>("edge1", float3(1, 0, 0));
  auto edge2 = getParam<float3>("edge2", float3(0, 1, 0));
  auto intensity = std::clamp(getParam<float>("intensity", 1.f),
      0.f,
      std::numeric_limits<float>::max());

  auto ol = osprayLight();
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "position", OSP_VEC3F, &position);
  ospSetParam(ol, "edge1", OSP_VEC3F, &edge1);
  ospSetParam(ol, "edge2", OSP_VEC3F, &edge2);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  ospCommit(ol);
}

} // namespace anari_ospray
