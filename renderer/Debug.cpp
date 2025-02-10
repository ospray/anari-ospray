// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Debug.h"

namespace anari_ospray {

Debug::Debug(OSPRayGlobalState *s) : Renderer(s, "debug")
{}

void Debug::commitParameters()
{
  Renderer::commitParameters();
  m_method = getParamString("method", "eyeLight");
}

void Debug::finalize()
{
  Renderer::finalize();

  auto ospR = osprayRenderer();
  ospSetParam(ospR, "method", OSP_STRING, m_method.c_str());
  ospCommit(ospR);
}

} // namespace anari_ospray
