// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Image2D.h"
#include "scene/surface/geometry/Geometry.h"

namespace anari_ospray {

Image2D::Image2D(OSPRayGlobalState *s) : Sampler(s), m_image(this)
{
  m_osprayTexture = ospNewTexture("texture2d");
}

bool Image2D::isValid() const
{
  return Sampler::isValid() && m_image;
}

void Image2D::commitParameters()
{
  Sampler::commitParameters();
  m_image = getParamObject<Array2D>("image");
  m_inAttribute =
      attributeFromString(getParamString("inAttribute", "attribute0"));
  m_filter = getParamString("filter", "linear");
}

void Image2D::finalize()
{
  if (!m_image) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'image' on image1D sampler");
    return;
  }

  auto ot = osprayTexture();
  auto format = OSP_TEXTURE_RGBA32F;
  ospSetParam(ot, "format", OSP_UINT, &format);
  auto filter =
      m_filter == "linear" ? OSP_TEXTURE_FILTER_LINEAR : OSP_TEXTURE_FILTER_NEAREST;
  ospSetParam(ot, "filter", OSP_UINT, &filter);

  auto unpackedColors = convertToColorArray(*m_image);
  auto size = m_image->size();
  auto d = ospNewSharedData2D(unpackedColors.data(), OSP_VEC4F, size.x, size.y);
  ospSetParam(ot, "data", OSP_DATA, &d);
  ospRelease(d);

  ospCommit(ot);

  m_unpackedColors = std::move(unpackedColors);
}

Attribute Image2D::inAttribute() const
{
  return m_inAttribute;
}

} // namespace anari_ospray
