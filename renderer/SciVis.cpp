// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "SciVis.h"

namespace anari_ospray {

SciVis::SciVis(OSPRayGlobalState *s) : Renderer(s, "scivis")
{
  commit();
}

void SciVis::commit()
{
  Renderer::commit();

  bool shadows = getParam<bool>("shadows", false);
  int aoSamples = getParam<int>("aoSamples", 0);
  float aoDistance = getParam<float>("aoDistance", 1e20f);
  float volumeSamplingRate = getParam<float>("volumeSamplingRate", 1.f);
  bool visibleLights = getParam<bool>("visibleLights", false);

  auto r = osprayRenderer();
  ospSetBool(r, "shadows", shadows);
  ospSetInt(r, "aoSamples", aoSamples);
  ospSetFloat(r, "aoDistance", aoDistance);
  ospSetFloat(r, "volumeSamplingRate", volumeSamplingRate);
  ospSetBool(r, "visibleLights", visibleLights);
  ospCommit(r);
}

} // namespace anari_ospray
