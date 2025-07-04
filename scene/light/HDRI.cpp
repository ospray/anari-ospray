// Copyright 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "HDRI.h"

namespace anari_ospray {

HDRI::HDRI(OSPRayGlobalState *s) : Light(s, "hdri"), m_image(this) {}

void HDRI::commitParameters()
{
  Light::commitParameters();
  m_up = getParam<float3>("up", float3(0, 0, 1));
  m_direction = getParam<float3>("direction", float3(1, 0, 0));
  intensity = getParam<float>("scale", 1.f);
  m_image = getParamObject<Array2D>("radiance");
}

void HDRI::finalize()
{
  if (!m_image) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "no radiance data provided to HDRI light");
    return;
  }

  if (m_image->elementType() != ANARI_FLOAT32_VEC3) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "radiance data for HDRI light needs to be of format FLOAT32_VEC3");
    return;
  }

  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "up", OSP_VEC3F, &m_up);
  ospSetParam(ol, "direction", OSP_VEC3F, &m_direction);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);

  OSPTexture ot = ospNewTexture("texture2d");
  auto format = OSP_TEXTURE_RGB32F;
  ospSetParam(ot, "format", OSP_UINT, &format);
  auto id = m_image->osprayData();
  ospSetParam(ot, "data", OSP_DATA, &id);
  ospCommit(ot);

  ospSetParam(ol, "map", OSP_TEXTURE, &ot);
  ospRelease(ot);

  ospCommit(ol);
}

bool HDRI::isValid() const
{
  return m_image;
}

} // namespace anari_ospray
