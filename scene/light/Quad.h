// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari_ospray {

struct QuadLight : public Light
{
  QuadLight(OSPRayGlobalState *d);
  void commitParameters() override;
  void finalize() override;

 private:
  float3 m_position{0, 0, 0};
  float3 m_edge1{1, 0, 0};
  float3 m_edge2{0, 1, 0};
};

} // namespace anari_ospray
