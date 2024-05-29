// Copyright 2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "HDRI.h"

namespace anari_ospray {

HDRI::HDRI(OSPRayGlobalState *s) : Light(s, "hdri") {}

void HDRI::commit()
{
  Light::commit();
  if (m_image)
    m_image->removeCommitObserver(this);

  auto up = getParam<float3>("up", float3(0, 0, 1));
  auto direction = getParam<float3>("direction", float3(1, 0, 0));
  intensity = getParam<float>("scale", 1.f);
  m_image = getParamObject<Array2D>("radiance");

  if (!m_image) {
    reportMessage(
        ANARI_SEVERITY_WARNING, "no radiance data provided to HDRI light");
    return;
  }
  m_image->addCommitObserver(this);

  auto ol = osprayLight();
  ospSetParam(ol, "visible", OSP_BOOL, &visible);
  ospSetParam(ol, "color", OSP_VEC3F, &color);
  ospSetParam(ol, "up", OSP_VEC3F, &up);
  ospSetParam(ol, "direction", OSP_VEC3F, &direction);
  ospSetParam(ol, "intensity", OSP_FLOAT, &intensity);

  OSPTexture ot = ospNewTexture("texture2d");
  auto format = OSP_TEXTURE_RGBA32F;
  ospSetParam(ot, "format", OSP_UINT, &format);
  auto filter = OSP_TEXTURE_FILTER_LINEAR;
  ospSetParam(ot, "filter", OSP_UINT, &filter);

  auto unpackedColors = convertToColorArray(*m_image);
  auto id = m_image->osprayData();
  auto size = m_image->size();
  auto d = ospNewSharedData2D(unpackedColors.data(), OSP_VEC4F, size.x, size.y);
  ospSetParam(ot, "data", OSP_DATA, &d);
  ospRelease(d);

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
