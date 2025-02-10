// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Light.h"
// subtypes
#include "Directional.h"
#include "HDRI.h"
#include "Point.h"
#include "Quad.h"
#include "Spot.h"

namespace anari_ospray {

Light::Light(OSPRayGlobalState *s, const char *osptype) : Object(ANARI_LIGHT, s)
{
  m_osprayLight = ospNewLight(osptype);
}

Light::~Light()
{
  ospRelease(m_osprayLight);
}

Light *Light::createInstance(std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "directional")
    return new Directional(s);
  else if (subtype == "hdri")
    return new HDRI(s);
  else if (subtype == "point")
    return new Point(s);
  else if (subtype == "quad")
    return new QuadLight(s);
  else if (subtype == "spot" || subtype == "ring")
    return new Spot(s);
  else
    return (Light *)new UnknownObject(ANARI_LIGHT, s);
}

void Light::commitParameters()
{
  visible = getParam<bool>("visible", true);
  color = getParam<float3>("color", float3(1, 1, 1));
  intensity = 1.f;
}

void Light::markFinalized()
{
  Object::markFinalized();
  deviceState()->objectUpdates.lastBLSCommitSceneRequest =
      helium::newTimeStamp();
}

OSPLight Light::osprayLight() const
{
  return m_osprayLight;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Light *);
