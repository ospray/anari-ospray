// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "PBM.h"

namespace anari_ospray {

PBM::PBM(OSPRayGlobalState *s)
    : Material(s, "principled"),
      m_normalSampler(this),
      m_clearcoatNormalSampler(this),
      m_opacitySampler(this),
      m_metallicSampler(this),
      m_roughnessSampler(this),
      m_specularSampler(this),
      m_clearcoatSampler(this),
      m_clearcoatRoughnessSampler(this),
      m_transmissionSampler(this),
      m_iorSampler(this),
      m_thicknessSampler(this),
      m_sheenColorSampler(this),
      m_sheenRoughnessSampler(this),
      m_emissiveSampler(this)
{}

void PBM::commitParameters()
{
  m_color = getParam<float3>("baseColor", float3(1.f));
  m_colorAttribute = attributeFromString(getParamString("baseColor", "none"));
  m_colorSampler = getParamObject<Sampler>("baseColor");

  m_normalSampler = getParamObject<Sampler>("normal");
  m_clearcoatNormalSampler = getParamObject<Sampler>("clearcoatNormal");

  m_alphaMode = getParamString("alphaMode", "opaque");
  m_opacity = getParam<float>("opacity", 1.f);
  m_opacitySampler = getParamObject<Sampler>("opacity");
  m_metallic = getParam<float>("metallic", 1.f);
  m_metallicSampler = getParamObject<Sampler>("metallic");
  m_roughness = getParam<float>("roughness", 1.f);
  m_roughnessSampler = getParamObject<Sampler>("roughness");
  m_specular = getParam<float>("specular", 0.f);
  m_specularSampler = getParamObject<Sampler>("specular");
  m_clearcoat = getParam<float>("clearcoat", 0.f);
  m_clearcoatSampler = getParamObject<Sampler>("clearcoat");
  m_clearcoatRoughness = getParam<float>("clearcoatRoughness", 0.f);
  m_clearcoatRoughnessSampler = getParamObject<Sampler>("clearcoatRoughness");
  m_transmission = getParam<float>("transmission", 0.f);
  m_transmissionSampler = getParamObject<Sampler>("transmission");
  m_ior = getParam<float>("ior", 1.5);
  m_iorSampler = getParamObject<Sampler>("ior");
  m_thickness = getParam<float>("thickness", 0.f);
  m_thicknessSampler = getParamObject<Sampler>("thickness");
  m_attenuationDistance = getParam<float>("attenuationDistance", INFINITY);
  m_attenuationColor = getParam<float3>("attenuationColor", float3(1.f));
  m_sheen = getParam<float3>("sheenColor", float3(0.f));
  m_sheenColorSampler = getParamObject<Sampler>("sheenColor");
  m_sheenRoughness = getParam<float>("sheenRoughness", 0.f);
  m_sheenRoughnessSampler = getParamObject<Sampler>("sheenRoughness");
  m_emissive = getParam<float3>("emissive", float3(0.f));
  m_emissiveSampler = getParamObject<Sampler>("emissive");
}

void PBM::finalize()
{
  const Sampler *opacitySampler =
      m_alphaMode == "opaque" ? nullptr : m_opacitySampler.get();

  // one shared texcoord stream for all maps; samplers must agree on attribute
  const Sampler *samplers[] = {m_colorSampler.get(),
      opacitySampler,
      m_metallicSampler.get(),
      m_roughnessSampler.get(),
      m_normalSampler.get(),
      m_emissiveSampler.get(),
      m_specularSampler.get(),
      m_clearcoatSampler.get(),
      m_clearcoatRoughnessSampler.get(),
      m_clearcoatNormalSampler.get(),
      m_transmissionSampler.get(),
      m_iorSampler.get(),
      m_sheenColorSampler.get(),
      m_sheenRoughnessSampler.get()};
  m_texcoordAttribute = Attribute::NONE;
  for (const auto *s : samplers) {
    if (!s || !s->isValid())
      continue;
    if (m_texcoordAttribute == Attribute::NONE)
      m_texcoordAttribute = s->inAttribute();
    else if (s->inAttribute() != m_texcoordAttribute)
      reportMessage(ANARI_SEVERITY_WARNING,
          "samplers on 'physicallyBased' material use different 'inAttribute's;"
          " a single texture coordinate stream is shared by all texture maps");
  }

  auto om = osprayMaterial();

  // OSPRay multiplies factor * map; use an identity factor when a map is bound
  auto setScalar = [&](const char *name, const char *mapName, float v,
                       const Sampler *s) {
    float f = (s && s->isValid()) ? 1.f : v;
    ospSetParam(om, name, OSP_FLOAT, &f);
    setSamplerMap(om, mapName, s);
  };
  auto setColor = [&](const char *name, const char *mapName, float3 v,
                      const Sampler *s) {
    float3 f = (s && s->isValid()) ? float3(1.f) : v;
    ospSetParam(om, name, OSP_VEC3F, &f);
    setSamplerMap(om, mapName, s);
  };

  setColor("baseColor", "map_baseColor", m_color, m_colorSampler.get());
  setScalar("opacity",
      "map_opacity",
      m_alphaMode == "opaque" ? 1.f : m_opacity,
      opacitySampler);
  setScalar("metallic", "map_metallic", m_metallic, m_metallicSampler.get());
  setScalar(
      "roughness", "map_roughness", m_roughness, m_roughnessSampler.get());
  setScalar("specular", "map_specular", m_specular, m_specularSampler.get());
  bool specularMetallic = false;
  ospSetParam(om, "specularMetallic", OSP_BOOL, &specularMetallic);
  setScalar("coat", "map_coat", m_clearcoat, m_clearcoatSampler.get());
  setScalar("coatRoughness",
      "map_coatRoughness",
      m_clearcoatRoughness,
      m_clearcoatRoughnessSampler.get());
  setScalar("transmission",
      "map_transmission",
      m_transmission,
      m_transmissionSampler.get());
  setScalar("ior", "map_ior", m_ior, m_iorSampler.get());

  // Volumes use ray-traced distance, not thickness maps.
  const bool hasThicknessSampler =
      m_thicknessSampler && m_thicknessSampler->isValid();
  bool thin = m_thickness <= 0.f && !hasThicknessSampler;
  ospSetParam(om, "thin", OSP_BOOL, &thin);
  setScalar("thickness", "map_thickness", m_thickness, nullptr);

  ospSetParam(om, "transmissionDepth", OSP_FLOAT, &m_attenuationDistance);
  ospSetParam(om, "transmissionColor", OSP_VEC3F, &m_attenuationColor);

  // OSPRay gates the sheen lobe on a scalar amount separate from its color
  bool hasSheen = sum(m_sheen) > 0.0f
      || (m_sheenColorSampler && m_sheenColorSampler->isValid());
  float sheenAmount = hasSheen ? 1.0f : 0.0f;
  ospSetParam(om, "sheen", OSP_FLOAT, &sheenAmount);
  setColor("sheenColor", "map_sheenColor", m_sheen, m_sheenColorSampler.get());
  setScalar("sheenRoughness",
      "map_sheenRoughness",
      m_sheenRoughness,
      m_sheenRoughnessSampler.get());

  setColor(
      "emissiveColor", "map_emissiveColor", m_emissive, m_emissiveSampler.get());
  setSamplerMap(om, "map_normal", m_normalSampler.get());
  setSamplerMap(om, "map_coatNormal", m_clearcoatNormalSampler.get());
  ospCommit(om);
}

} // namespace anari_ospray
