// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct Pathtracer : public Renderer
{
  Pathtracer(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;

 private:
  int m_lightSamples{-1};
  int m_roulettePathLength{5};
  int m_maxScatteringEvents{20};
  float m_maxContribution{1e20f};
  bool m_backgroundRefraction{false};
};

} // namespace anari_ospray
