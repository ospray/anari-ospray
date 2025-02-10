// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct Debug : public Renderer
{
  Debug(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;

 private:
  std::string m_method;
};

} // namespace anari_ospray
