// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace anari_ospray {

struct Perspective : public Camera
{
  Perspective(OSPRayGlobalState *s);
  void commit() override;
};

} // namespace anari_ospray
