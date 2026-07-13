// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Material.h"

namespace anari_ospray {

struct Matte : public Material
{
  Matte(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;

 private:
  helium::ChangeObserverPtr<Sampler> m_opacitySampler;
  std::string m_alphaMode{"opaque"};
  float m_opacity{1.f};
};

} // namespace anari_ospray
