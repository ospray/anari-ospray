// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"
// subtypes
#include "Matte.h"
#include "PBM.h"
// std
#include <string>

namespace anari_ospray {

Material::Material(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_MATERIAL, s), m_colorSampler(this)
{
  m_osprayMaterial = ospNewMaterial(osptype);
}

Material::~Material()
{
  ospRelease(m_osprayMaterial);
}

Material *Material::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "matte")
    return new Matte(s);
  else if (subtype == "physicallyBased")
    return new PBM(s);
  else
    return (Material *)new UnknownObject(ANARI_MATERIAL, s);
}

OSPMaterial Material::osprayMaterial() const
{
  return m_osprayMaterial;
}

void Material::setSamplerMap(
    OSPMaterial om, const char *mapName, const Sampler *s)
{
  if (s && s->isValid()) {
    auto tex = s->osprayTexture();
    ospSetParam(om, mapName, OSP_TEXTURE, &tex);
    s->applyInTransform(om, mapName);
  } else {
    const auto transformName = std::string(mapName) + ".transform";
    const auto translationName = std::string(mapName) + ".translation";
    ospRemoveParam(om, mapName);
    ospRemoveParam(om, transformName.c_str());
    ospRemoveParam(om, translationName.c_str());
  }
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Material *);
