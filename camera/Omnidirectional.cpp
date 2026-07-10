// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Omnidirectional.h"

namespace anari_ospray {

Omnidirectional::Omnidirectional(OSPRayGlobalState *s) : Camera(s, "panoramic") {}

void Omnidirectional::finalize()
{
  Camera::finalize();
  // stereo (panoramic has no depth of field)
  ospSetParam(osprayCamera(), "stereoMode", OSP_UINT, &m_stereoMode);
  ospSetParam(osprayCamera(),
      "interpupillaryDistance",
      OSP_FLOAT,
      &m_interpupillaryDistance);
  ospCommit(osprayCamera());
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Omnidirectional *);
