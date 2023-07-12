// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
#include "sampler/Sampler.h"

namespace anari_ospray {

struct Material : public Object
{
  Material(OSPRayGlobalState *s, const char *osptype);
  ~Material() override;

  static Material *createInstance(
      std::string_view subtype, OSPRayGlobalState *s);

  float3 color() const;
  Attribute colorAttribute() const;
  const Sampler *colorSampler() const;
  Attribute texcoordAttribute() const;

  OSPMaterial osprayMaterial() const;

 protected:
  float3 m_color{1.f, 1.f, 1.f};
  Attribute m_colorAttribute{Attribute::NONE};
  Attribute m_texcoordAttribute{Attribute::NONE};
  helium::IntrusivePtr<Sampler> m_colorSampler;

  OSPMaterial m_osprayMaterial{nullptr};
};

// Inlined definitions ////////////////////////////////////////////////////////

inline float3 Material::color() const
{
  return m_color;
}

inline Attribute Material::colorAttribute() const
{
  return m_colorAttribute;
}

inline Attribute Material::texcoordAttribute() const
{
  return m_texcoordAttribute;
}

inline const Sampler *Material::colorSampler() const
{
  return m_colorSampler.ptr;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Material *, ANARI_MATERIAL);
