// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace anari_ospray {

struct Orthographic : public Camera
{
  Orthographic(OSPRayGlobalState *s);
  void commit() override;
};

} // namespace anari_ospray
