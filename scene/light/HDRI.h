// Copyright 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Light.h"
#include "array/Array2D.h"

namespace anari_ospray {

struct HDRI : public Light
{
  HDRI(OSPRayGlobalState *d);
  void commit() override;
  bool isValid() const override;

 private:
  helium::IntrusivePtr<Array2D> m_image;
};

} // namespace anari_ospray
