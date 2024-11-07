// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Matte.h"

namespace anari_ospray {

Matte::Matte(OSPRayGlobalState *s) : Material(s, "obj") {}

void Matte::commit()
{
  m_color = getParam<float3>("color", float3(1.f));
  m_colorAttribute = attributeFromString(getParamString("color", "none"));
  m_colorSampler = getParamObject<Sampler>("color");

  auto opacity = getParam<float>("opacity", 1.f);
  auto specular = getParam<float>("specular", 0.f);
  auto specularColor = getParam<float3>("specularColor", float3(1.f));
  float specAdjust = 2.0f / (2.0f + specular);
  float3 specularf = { specularColor[0] * specAdjust, specularColor[1] * specAdjust,
        specularColor[2] * specAdjust };

  OSPTexture ot = nullptr;
  if (m_colorSampler && m_colorSampler->isValid()) {
    m_texcoordAttribute = m_colorSampler->inAttribute();
    ot = m_colorSampler->osprayTexture();
  } else
    m_texcoordAttribute = Attribute::NONE;

  auto om = osprayMaterial();
  ospSetParam(om, "kd", OSP_VEC3F, &m_color);
  if (ot)
    ospSetParam(om, "map_kd", OSP_TEXTURE, &ot);
  else
    ospRemoveParam(om, "map_kd");

  ospSetParam(om, "ks", OSP_VEC3F, &specularf);
  ospSetParam(om, "ns", OSP_FLOAT, &specular);
  ospSetParam(om, "d", OSP_FLOAT, &opacity);

  ospCommit(om);
}

} // namespace anari_ospray
