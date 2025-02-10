// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari_ospray {

struct Point : public Light
{
  Point(OSPRayGlobalState *d);
  void commitParameters() override;
  void finalize() override;

 private:
  float3 m_position{0, 0, 0};
  float m_radius{0.f};
};

} // namespace anari_ospray
