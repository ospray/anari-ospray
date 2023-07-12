// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Material.h"
// subtypes
#include "Matte.h"
#include "PBM.h"

namespace anari_ospray {

Material::Material(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_MATERIAL, s)
{
  s->objectCounts.materials++;
  m_osprayMaterial = ospNewMaterial("", osptype);
}

Material::~Material()
{
  ospRelease(m_osprayMaterial);
  deviceState()->objectCounts.materials--;
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

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Material *);
