// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"

namespace anari_ospray {

struct Directional : public Light
{
  Directional(OSPRayGlobalState *d);
  void commit() override;
};

} // namespace anari_ospray
