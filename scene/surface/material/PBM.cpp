// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "PBM.h"

namespace anari_ospray {

PBM::PBM(OSPRayGlobalState *s) : Material(s, "principled") {}

void PBM::commit()
{
  m_color = getParam<float3>("baseColor", float3(1.f));
  m_colorAttribute = attributeFromString(getParamString("baseColor", "none"));
  m_colorSampler = getParamObject<Sampler>("baseColor");

  auto opacity = getParam<float>("opacity", 1.f);
  auto metallic = getParam<float>("metallic", 1.f);
  auto roughness = getParam<float>("roughness", 1.f);
  auto specular = getParam<float>("specular", 0.f);
  auto clearcoat = getParam<float>("clearcoat", 0.f);
  auto clearcoatRoughness = getParam<float>("clearcoatRoughness", 0.f);
  auto transmission = getParam<float>("transmission", 0.f);
  auto ior = getParam<float>("ior", 1.5);
  auto thickness = getParam<float>("thickness", 0.f);

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
  ospSetParam(om, "opacity", OSP_FLOAT, &opacity);
  ospSetParam(om, "metallic", OSP_FLOAT, &metallic);
  ospSetParam(om, "roughness", OSP_FLOAT, &roughness);
  ospSetParam(om, "specular", OSP_FLOAT, &specular);
  ospSetParam(om, "coat", OSP_FLOAT, &clearcoat);
  ospSetParam(om, "coatRoughness", OSP_FLOAT, &clearcoatRoughness);
  ospSetParam(om, "transmission", OSP_FLOAT, &transmission);
  ospSetParam(om, "ior", OSP_FLOAT, &ior);
  ospSetParam(om, "thickness", OSP_FLOAT, &thickness);
  ospCommit(om);
}

} // namespace anari_ospray
