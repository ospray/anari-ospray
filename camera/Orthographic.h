// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Camera.h"

namespace anari_ospray {

struct Orthographic : public Camera
{
  Orthographic(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;
  void setFrameAspect(float frameAspect) override;

 private:
  float m_height{1.f};
};

} // namespace anari_ospray
