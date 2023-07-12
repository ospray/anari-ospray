// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Material.h"

namespace anari_ospray {

struct Matte : public Material
{
  Matte(OSPRayGlobalState *s);
  void commit() override;
};

} // namespace anari_ospray
