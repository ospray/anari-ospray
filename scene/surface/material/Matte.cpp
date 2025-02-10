// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Matte.h"

namespace anari_ospray {

Matte::Matte(OSPRayGlobalState *s) : Material(s, "obj") {}

void Matte::commitParameters()
{
  m_color = getParam<float3>("color", float3(0.8f));
  m_colorAttribute = attributeFromString(getParamString("color", "none"));
  m_colorSampler = getParamObject<Sampler>("color");
  m_opacity = getParam<float>("opacity", 1.f);
}

void Matte::finalize()
{
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

  ospSetParam(om, "d", OSP_FLOAT, &m_opacity);

  ospCommit(om);
}

} // namespace anari_ospray
