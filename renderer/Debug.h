// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Renderer.h"

namespace anari_ospray {

struct Debug : public Renderer
{
  Debug(OSPRayGlobalState *s);
  virtual void commit() override;
};

} // namespace anari_ospray
