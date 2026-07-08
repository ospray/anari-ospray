// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Omnidirectional.h"

namespace anari_ospray {

Omnidirectional::Omnidirectional(OSPRayGlobalState *s) : Camera(s, "panoramic") {}

void Omnidirectional::finalize()
{
  Camera::finalize();
  ospCommit(osprayCamera());
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Omnidirectional *);
