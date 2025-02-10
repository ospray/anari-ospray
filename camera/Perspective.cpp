// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace anari_ospray {

Perspective::Perspective(OSPRayGlobalState *s) : Camera(s, "perspective") {}

void Perspective::commitParameters()
{
  Camera::commitParameters();
  // NOTE: demonstrate alternative 'raw' method for getting parameter values
  if (!getParam("fovy", ANARI_FLOAT32, &m_fovy))
    m_fovy = radians(60.f);
  m_fovy = degrees(m_fovy);
  m_aspect = getParam<float>("aspect", 1.f);
}

void Perspective::finalize()
{
  Camera::finalize();
  ospSetParam(osprayCamera(), "fovy", OSP_FLOAT, &m_fovy);
  ospSetParam(osprayCamera(), "aspect", OSP_FLOAT, &m_aspect);
  ospCommit(osprayCamera());
}

} // namespace anari_ospray
