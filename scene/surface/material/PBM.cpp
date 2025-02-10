// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "PBM.h"

namespace anari_ospray {

PBM::PBM(OSPRayGlobalState *s) : Material(s, "principled") {}

void PBM::commitParameters()
{
  m_color = getParam<float3>("baseColor", float3(1.f));
  m_colorAttribute = attributeFromString(getParamString("baseColor", "none"));
  m_colorSampler = getParamObject<Sampler>("baseColor");

  m_opacity = getParam<float>("opacity", 1.f);
  m_metallic = getParam<float>("metallic", 1.f);
  m_roughness = getParam<float>("roughness", 1.f);
  m_specular = getParam<float>("specular", 0.f);
  m_clearcoat = getParam<float>("clearcoat", 0.f);
  m_clearcoatRoughness = getParam<float>("clearcoatRoughness", 0.f);
  m_transmission = getParam<float>("transmission", 0.f);
  m_ior = getParam<float>("ior", 1.5);
  m_thickness = getParam<float>("thickness", 0.f);
  m_attenuationDistance = getParam<float>("attenuationDistance", INFINITY);
  m_attenuationColor = getParam<float3>("attenuationColor", float3(1.f));
  m_sheen = getParam<float3>("sheen", float3(0.f));
  m_sheenRoughness = getParam<float>("sheenRoughness", 0.f);
  m_emissive = getParam<float3>("emissive", float3(0.f));
}

void PBM::finalize()
{
  OSPTexture ot = nullptr;
  if (m_colorSampler && m_colorSampler->isValid()) {
    m_texcoordAttribute = m_colorSampler->inAttribute();
    ot = m_colorSampler->osprayTexture();
  } else
    m_texcoordAttribute = Attribute::NONE;

  auto om = osprayMaterial();
  ospSetParam(om, "baseColor", OSP_VEC3F, &m_color);
  if (ot)
    ospSetParam(om, "map_baseColor", OSP_TEXTURE, &ot);
  else
    ospRemoveParam(om, "map_baseColor");
  ospSetParam(om, "opacity", OSP_FLOAT, &m_opacity);
  ospSetParam(om, "metallic", OSP_FLOAT, &m_metallic);
  ospSetParam(om, "roughness", OSP_FLOAT, &m_roughness);
  ospSetParam(om, "specular", OSP_FLOAT, &m_specular);
  bool b = false;
  ospSetParam(om, "specularMetallic", OSP_BOOL, &b);
  ospSetParam(om, "coat", OSP_FLOAT, &m_clearcoat);
  ospSetParam(om, "coatRoughness", OSP_FLOAT, &m_clearcoatRoughness);
  ospSetParam(om, "transmission", OSP_FLOAT, &m_transmission);
  if (m_transmission) {
    bool b = m_thickness > 0.0f;
    ospSetParam(om, "thin", OSP_BOOL, &b);
    ospSetParam(om, "transmissionColor", OSP_VEC3F, &m_color);
  }
  ospSetParam(om, "ior", OSP_FLOAT, &m_ior);
  ospSetParam(om, "thickness", OSP_FLOAT, &m_thickness);
  ospSetParam(om, "transmissionDepth", OSP_FLOAT, &m_attenuationDistance);
  ospSetParam(om, "transmissionColor", OSP_VEC3F, &m_attenuationColor);
  if (sum(m_sheen) > 0.0f) {
    float f = 1.0f;
    ospSetParam(om, "sheen", OSP_FLOAT, &f);
  }
  ospSetParam(om, "sheenColor", OSP_VEC3F, &m_sheen);
  ospSetParam(om, "sheenRoughness", OSP_FLOAT, &m_sheenRoughness);
  ospSetParam(om, "emissiveColor", OSP_VEC3F, &m_emissive);
  ospCommit(om);
}

} // namespace anari_ospray
