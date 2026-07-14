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
  void commitParameters() override;
  void finalize() override;

  Attribute inAttribute() const override;

 private:
  helium::ChangeObserverPtr<Array2D> m_image;
  Attribute m_inAttribute{Attribute::NONE};
  std::string m_filter;
  uint32_t m_wrapMode[2]{
      OSP_TEXTURE_WRAP_CLAMP_TO_EDGE, OSP_TEXTURE_WRAP_CLAMP_TO_EDGE};
  std::vector<float4> m_unpackedColors;
};

} // namespace anari_ospray
