// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "SciVis.h"

namespace anari_ospray {

SciVis::SciVis(OSPRayGlobalState *s)
    : Renderer(s, s->distributed ? "mpiRaycast" : "scivis")
{
  commit();
}

void SciVis::commit()
{
  Renderer::commit();

  auto r = osprayRenderer();
  if (!deviceState()->distributed) {
    ospSetBool(r, "shadows", getParam<bool>("shadows", false));
    ospSetBool(r, "visibleLights", getParam<bool>("visibleLights", false));
  }
  ospSetInt(r, "aoSamples", getParam<int>("aoSamples", 0));
  ospSetFloat(r, "aoDistance", getParam<float>("aoDistance", 1e20f));
  ospSetFloat(
      r, "volumeSamplingRate", getParam<float>("volumeSamplingRate", 1.f));
  ospCommit(r);
}

} // namespace anari_ospray
