// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct SciVis : public Renderer
{
  SciVis(OSPRayGlobalState *s);
  virtual void commit() override;
};

} // namespace anari_ospray
