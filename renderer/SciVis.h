// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct SciVis : public Renderer
{
  SciVis(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;

 private:
  bool m_shadows{false};
  bool m_visibleLights{false};
  int m_aoSamples{0};
  float m_aoDistance{1e20f};
  float m_volumeSamplingRate{1.f};
};

} // namespace anari_ospray
