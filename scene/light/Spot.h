// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari_ospray {

struct Spot : public Light
{
  Spot(OSPRayGlobalState *d);
  void commitParameters() override;
  void finalize() override;

 private:
  float3 m_position{0, 0, 0};
  float3 m_direction{0, 0, -1};
  float m_openingAngle{M_PI};
  float m_falloffAngle{0.1f};
  float m_radius{0.f};
  float m_innerRadius{0.f};
};

} // namespace anari_ospray
