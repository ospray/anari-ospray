// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace anari_ospray {

Perspective::Perspective(OSPRayGlobalState *s) : Camera(s, "perspective") {}

void Perspective::commitParameters()
{
  Camera::commitParameters();
  m_fovy = degrees(getParam<float>("fovy", M_PI / 3.f));
}

void Perspective::finalize()
{
  Camera::finalize();
  ospSetParam(osprayCamera(), "fovy", OSP_FLOAT, &m_fovy);
  // depth of field + stereo
  ospSetParam(osprayCamera(), "apertureRadius", OSP_FLOAT, &m_apertureRadius);
  ospSetParam(osprayCamera(), "focusDistance", OSP_FLOAT, &m_focusDistance);
  ospSetParam(osprayCamera(), "stereoMode", OSP_UINT, &m_stereoMode);
  ospSetParam(osprayCamera(),
      "interpupillaryDistance",
      OSP_FLOAT,
      &m_interpupillaryDistance);
  ospCommit(osprayCamera());
}

void Perspective::setFrameAspect(float frameAspect)
{
  applyAspect(frameAspect);
}

} // namespace anari_ospray
