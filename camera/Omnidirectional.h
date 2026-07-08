// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"
// std
#include <string>

namespace anari_ospray {

struct Omnidirectional : public Camera
{
  Omnidirectional(OSPRayGlobalState *s);
  void finalize() override;
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Omnidirectional *, ANARI_CAMERA);
