// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Orthographic.h"

namespace anari_ospray {

Orthographic::Orthographic(OSPRayGlobalState *s) : Camera(s, "orthographic") {}

void Orthographic::commitParameters()
{
  Camera::commitParameters();
  m_height = getParam<float>("height", 1.f);
}

void Orthographic::finalize()
{
  Camera::finalize();
  ospSetParam(osprayCamera(), "height", OSP_FLOAT, &m_height);
  ospCommit(osprayCamera());
}

void Orthographic::setFrameAspect(float frameAspect)
{
  applyAspect(frameAspect);
}

} // namespace anari_ospray
