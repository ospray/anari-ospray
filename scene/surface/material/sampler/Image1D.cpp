// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Image1D.h"
#include "scene/surface/geometry/Geometry.h"

namespace anari_ospray {

Image1D::Image1D(OSPRayGlobalState *s) : Sampler(s)
{
  m_osprayTexture = ospNewTexture("texture2d");
}

bool Image1D::isValid() const
{
  return Sampler::isValid() && m_image;
}

void Image1D::commit()
{
  Sampler::commit();
  m_image = getParamObject<Array1D>("image");
  m_inAttribute =
      attributeFromString(getParamString("inAttribute", "attribute0"));
  auto linearFilter = getParamString("filter", "linear") != "nearest";

  if (!m_image) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'image' on image1D sampler");
    return;
  }

  auto ot = osprayTexture();
  auto format = OSP_TEXTURE_RGBA32F;
  ospSetParam(ot, "format", OSP_INT, &format);
  auto filter =
      linearFilter ? OSP_TEXTURE_FILTER_BILINEAR : OSP_TEXTURE_FILTER_NEAREST;
  ospSetParam(ot, "filter", OSP_INT, &filter);

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
