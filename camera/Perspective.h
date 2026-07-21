// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace anari_ospray {

struct Perspective : public Camera
{
  Perspective(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;
  void setFrameAspect(float frameAspect) override;

 private:
  float m_fovy{M_PI / 3.f};
};

} // namespace anari_ospray
