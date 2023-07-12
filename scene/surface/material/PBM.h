// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Material.h"

namespace anari_ospray {

struct PBM : public Material
{
  PBM(OSPRayGlobalState *s);
  void commit() override;
};

} // namespace anari_ospray
