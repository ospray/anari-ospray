// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Sampler.h"
#include "array/Array2D.h"

namespace anari_ospray {

struct Image2D : public Sampler
{
  Image2D(OSPRayGlobalState *d);

  bool isValid() const override;
  void commit() override;

  Attribute inAttribute() const override;

 private:
  helium::IntrusivePtr<Array2D> m_image;
  Attribute m_inAttribute{Attribute::NONE};
  bool m_linearFilter{true};
  std::vector<float4> m_unpackedColors;
};

} // namespace anari_ospray
