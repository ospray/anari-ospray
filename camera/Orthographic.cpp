// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Orthographic.h"

namespace anari_ospray {

Orthographic::Orthographic(OSPRayGlobalState *s) : Camera(s, "orthographic") {}

void Orthographic::commitParameters()
{
  Camera::commitParameters();
  m_aspect = getParam<float>("aspect", 1.f);
  m_height = getParam<float>("height", 1.f);
}

void Orthographic::finalize()
{
  Camera::finalize();
  ospSetParam(osprayCamera(), "aspect", OSP_FLOAT, &m_aspect);
  ospSetParam(osprayCamera(), "height", OSP_FLOAT, &m_height);
  ospCommit(osprayCamera());
}

} // namespace anari_ospray
