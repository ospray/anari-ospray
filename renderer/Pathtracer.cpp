// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Pathtracer.h"

namespace anari_ospray {

Pathtracer::Pathtracer(OSPRayGlobalState *s) : Renderer(s, "pathtracer")
{
  commit();
}

void Pathtracer::commit()
{
  Renderer::commit();

  bool lightSamples = getParam<int>("lightSamples", -1);
  int roulettePathDepth = getParam<int>("roulettePathDepth", 5);
  int maxScatteringEvents = getParam<int>("maxScatteringEvents", 20);
  float maxContribution = getParam<float>("maxContribution", 1e20f);
  bool backgroundRefraction = getParam<bool>("backgroundRefraction", false);

  auto r = osprayRenderer();
  ospSetInt(r, "lightSamples", lightSamples);
  ospSetInt(r, "roulettePathDepth", roulettePathDepth);
  ospSetInt(r, "maxScatteringEvents", maxScatteringEvents);
  ospSetFloat(r, "maxContribution", maxContribution);
  ospSetBool(r, "backgroundRefraction", backgroundRefraction);
  ospCommit(r);
}

} // namespace anari_ospray
