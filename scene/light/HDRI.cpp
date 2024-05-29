// Copyright 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "HDRI.h"

namespace anari_ospray {

HDRI::HDRI(OSPRayGlobalState *s) : Light(s, "hdri") {}

void HDRI::commit()
{
  Light::commit();
  if (m_image)
    m_image->removeChangeObserver(this);

  auto up = getParam<float3>("up", float3(0, 0, 1));
  auto direction = getParam<float3>("direction", float3(1, 0, 0));
  intensity = getParam<float>("scale", 1.f);
  m_image = getParamObject<Array2D>("radiance");

  if (!m_image) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "no radiance data provided to HDRI light");
    return;
  }
  m_image->addChangeObserver(this);

  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "up", OSP_VEC3F, &up);
  ospSetParam(ol, "direction", OSP_VEC3F, &direction);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);
  auto id = m_image->osprayData();
  ospSetParam(ol, "map", OSP_DATA, &id);
  ospCommit(ol);
}

bool HDRI::isValid() const
{
  return m_image;
}

} // namespace anari_ospray
