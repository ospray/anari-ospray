// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "SciVis.h"

namespace anari_ospray {

SciVis::SciVis(OSPRayGlobalState *s)
    : Renderer(s, s->distributed ? "mpiRaycast" : "scivis")
{}

void SciVis::commitParameters()
{
  Renderer::commitParameters();
  m_shadows = getParam<bool>("shadows", false);
  m_visibleLights = getParam<bool>("visibleLights", false);
  m_aoSamples = getParam<int>("aoSamples", 0);
  m_aoDistance = getParam<float>("aoDistance", 1e20f);
  m_volumeSamplingRate = getParam<float>("volumeSamplingRate", 1.f);
}

void SciVis::finalize()
{
  Renderer::finalize();

  auto r = osprayRenderer();
  if (!deviceState()->distributed) {
    ospSetBool(r, "shadows", m_shadows);
    ospSetBool(r, "visibleLights", m_visibleLights);
  }
  ospSetInt(r, "aoSamples", m_aoSamples);
  ospSetFloat(r, "aoDistance", m_aoDistance);
  ospSetFloat(r, "volumeSamplingRate", m_volumeSamplingRate);
  ospCommit(r);
}

} // namespace anari_ospray
