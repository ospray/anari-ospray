// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Material.h"

namespace anari_ospray {

struct PBM : public Material
{
  PBM(OSPRayGlobalState *s);
  void commitParameters() override;
  void finalize() override;

 private:
  helium::ChangeObserverPtr<Sampler> m_normalSampler;
  helium::ChangeObserverPtr<Sampler> m_clearcoatNormalSampler;
  helium::ChangeObserverPtr<Sampler> m_opacitySampler;
  helium::ChangeObserverPtr<Sampler> m_metallicSampler;
  helium::ChangeObserverPtr<Sampler> m_roughnessSampler;
  helium::ChangeObserverPtr<Sampler> m_specularSampler;
  helium::ChangeObserverPtr<Sampler> m_clearcoatSampler;
  helium::ChangeObserverPtr<Sampler> m_clearcoatRoughnessSampler;
  helium::ChangeObserverPtr<Sampler> m_transmissionSampler;
  helium::ChangeObserverPtr<Sampler> m_iorSampler;
  helium::ChangeObserverPtr<Sampler> m_thicknessSampler;
  helium::ChangeObserverPtr<Sampler> m_sheenColorSampler;
  helium::ChangeObserverPtr<Sampler> m_sheenRoughnessSampler;
  helium::ChangeObserverPtr<Sampler> m_emissiveSampler;

  float m_opacity{1.f};
  float m_metallic{1.f};
  float m_roughness{1.f};
  float m_specular{0.f};
  float m_clearcoat{0.f};
  float m_clearcoatRoughness{0.f};
  float m_transmission{0.f};
  float m_ior{1.5f};
  float m_thickness{0.f};
  float m_attenuationDistance{INFINITY};
  float3 m_attenuationColor{1.f};
  float3 m_sheen{0.f};
  float m_sheenRoughness{0.f};
  float3 m_emissive{0.f};
};

} // namespace anari_ospray
