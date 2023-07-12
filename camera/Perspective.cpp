// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Perspective.h"

namespace anari_ospray {

Perspective::Perspective(OSPRayGlobalState *s) : Camera(s, "perspective") {}

void Perspective::commit()
{
  Camera::commit();
  // NOTE: demonstrate alternative 'raw' method for getting parameter values
  float fovy = 0.f;
  if (!getParam("fovy", ANARI_FLOAT32, &fovy))
    fovy = radians(60.f);
  fovy = degrees(fovy);
  float aspect = getParam<float>("aspect", 1.f);
  ospSetParam(osprayCamera(), "fovy", OSP_FLOAT, &fovy);
  ospSetParam(osprayCamera(), "aspect", OSP_FLOAT, &aspect);
  ospCommit(osprayCamera());
}

} // namespace anari_ospray
