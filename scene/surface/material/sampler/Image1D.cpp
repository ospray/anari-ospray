// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Image1D.h"
#include "scene/surface/geometry/Geometry.h"

namespace anari_ospray {

Image1D::Image1D(OSPRayGlobalState *s) : Sampler(s), m_image(this)
{
  m_osprayTexture = ospNewTexture("texture2d");
}

bool Image1D::isValid() const
{
  return Sampler::isValid() && m_image;
}

void Image1D::commitParameters()
{
  Sampler::commitParameters();
  m_image = getParamObject<Array1D>("image");
  m_inAttribute =
      attributeFromString(getParamString("inAttribute", "attribute0"));
  m_filter = getParamString("filter", "linear");
}

void Image1D::finalize()
{
  if (!m_image) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'image' on image1D sampler");
    return;
  }

  auto ot = osprayTexture();
  auto format = OSP_TEXTURE_RGBA32F;
  ospSetParam(ot, "format", OSP_UINT, &format);
  auto filter = m_filter == "nearest" ? OSP_TEXTURE_FILTER_NEAREST
                                      : OSP_TEXTURE_FILTER_LINEAR;
  ospSetParam(ot, "filter", OSP_UINT, &filter);

  auto unpackedColors = convertToColorArray(*m_image);
  auto d = ospNewSharedData1D(
      unpackedColors.data(), OSP_VEC4F, unpackedColors.size());
  ospSetParam(ot, "data", OSP_DATA, &d);
  ospRelease(d);

  ospCommit(ot);

  m_unpackedColors = std::move(unpackedColors);
}

Attribute Image1D::inAttribute() const
{
  return m_inAttribute;
}

} // namespace anari_ospray
