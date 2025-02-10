// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Pathtracer.h"

namespace anari_ospray {

Pathtracer::Pathtracer(OSPRayGlobalState *s) : Renderer(s, "pathtracer")
{}

void Pathtracer::commitParameters()
{
  Renderer::commitParameters();
  m_lightSamples = getParam<int>("lightSamples", -1);
  m_roulettePathLength = getParam<int>("roulettePathLength", 5);
  m_maxScatteringEvents = getParam<int>("maxScatteringEvents", 20);
  m_maxContribution = getParam<float>("maxContribution", 1e20f);
  m_backgroundRefraction = getParam<bool>("backgroundRefraction", false);
}

void Pathtracer::finalize()
{
  Renderer::finalize();

  auto r = osprayRenderer();
  ospSetInt(r, "lightSamples", m_lightSamples);
  ospSetInt(r, "roulettePathLength", m_roulettePathLength);
  ospSetInt(r, "maxScatteringEvents", m_maxScatteringEvents);
  ospSetFloat(r, "maxContribution", m_maxContribution);
  ospSetBool(r, "backgroundRefraction", m_backgroundRefraction);
  ospCommit(r);
}

} // namespace anari_ospray
