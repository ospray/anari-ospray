// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari_ospray {

struct Directional : public Light
{
  Directional(OSPRayGlobalState *d);
  void commitParameters() override;
  void finalize() override;

 private:
  float3 m_direction{0, 0, -1};
  float m_angularDiameter{0.f};
};

} // namespace anari_ospray
