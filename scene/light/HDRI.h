// Copyright 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"
#include "array/Array2D.h"

namespace anari_ospray {

struct HDRI : public Light
{
  HDRI(OSPRayGlobalState *d);
  void commitParameters() override;
  void finalize() override;
  bool isValid() const override;

 private:
  helium::ChangeObserverPtr<Array2D> m_image;
  float3 m_up{0, 0, 1};
  float3 m_direction{1, 0, 0};
};

} // namespace anari_ospray
