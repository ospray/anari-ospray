// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct Pathtracer : public Renderer
{
  Pathtracer(OSPRayGlobalState *s);
  virtual void commit() override;
};

} // namespace anari_ospray
