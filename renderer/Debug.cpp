// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Debug.h"

namespace anari_ospray {

Debug::Debug(OSPRayGlobalState *s) : Renderer(s, "debug")
{
  commit();
}

void Debug::commit()
{
  Renderer::commit();

  auto method = getParamString("method", "eyeLight");

  auto ospR = osprayRenderer();
  ospSetParam(ospR, "method", OSP_STRING, method.c_str());
  ospCommit(ospR);
}

} // namespace anari_ospray
