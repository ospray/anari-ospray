// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Orthographic.h"

namespace anari_ospray {

Orthographic::Orthographic(OSPRayGlobalState *s) : Camera(s, "orthographic") {}

void Orthographic::commit()
{
  Camera::commit();
  const float aspect = getParam<float>("aspect", 1.f);
  const float height = getParam<float>("height", 1.f);
  ospSetParam(osprayCamera(), "aspect", OSP_FLOAT, &aspect);
  ospSetParam(osprayCamera(), "height", OSP_FLOAT, &height);
  ospCommit(osprayCamera());
}

} // namespace anari_ospray
