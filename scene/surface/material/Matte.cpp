// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Matte.h"

namespace anari_ospray {

Matte::Matte(OSPRayGlobalState *s) : Material(s, "obj"), m_opacitySampler(this)
{}

void Matte::commitParameters()
{
  m_color = getParam<float3>("color", float3(0.8f));
  m_colorAttribute = attributeFromString(getParamString("color", "none"));
  m_colorSampler = getParamObject<Sampler>("color");
  m_alphaMode = getParamString("alphaMode", "opaque");
  m_opacity = getParam<float>("opacity", 1.f);
  m_opacitySampler = getParamObject<Sampler>("opacity");
}

void Matte::finalize()
{
  const Sampler *opacitySampler =
      m_alphaMode == "opaque" ? nullptr : m_opacitySampler.get();

  // one shared texcoord stream for all maps; samplers must agree on attribute
  m_texcoordAttribute = Attribute::NONE;
  const Sampler *samplers[] = {m_colorSampler.get(), opacitySampler};
  for (const auto *s : samplers) {
    if (!s || !s->isValid())
      continue;
    if (m_texcoordAttribute == Attribute::NONE)
      m_texcoordAttribute = s->inAttribute();
    else if (s->inAttribute() != m_texcoordAttribute)
      reportMessage(ANARI_SEVERITY_WARNING,
          "samplers on 'matte' material use different 'inAttribute's;"
          " a single texture coordinate stream is shared by all texture maps");
  }

  auto om = osprayMaterial();

  // OSPRay multiplies factor * map; use an identity factor when a map is bound
  float3 kd = (m_colorSampler && m_colorSampler->isValid()) ? float3(1.f) : m_color;
  ospSetParam(om, "kd", OSP_VEC3F, &kd);
  setSamplerMap(om, "map_kd", m_colorSampler.get());

  float d = m_alphaMode == "opaque"
          || (opacitySampler && opacitySampler->isValid())
      ? 1.f
      : m_opacity;
  ospSetParam(om, "d", OSP_FLOAT, &d);
  setSamplerMap(om, "map_d", opacitySampler);

  ospCommit(om);
}

} // namespace anari_ospray
