// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Image1D.h"
#include "enumCast.hpp"
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
  m_wrapMode = wrapModeFromString(getParamString("wrapMode", "clampToEdge"));
}

void Image1D::finalize()
{
  if (!m_image) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'image' on image1D sampler");
    return;
  }

  auto ot = osprayTexture();

  // prefer the native texel format (zero-copy); baking the out-transform
  // requires unpacking, so fall back to RGBA32F in that case
  OSPTextureFormat format = enumCast<OSPTextureFormat>(m_image->elementType());
  auto filter = m_filter == "nearest" ? OSP_TEXTURE_FILTER_NEAREST
                                      : OSP_TEXTURE_FILTER_LINEAR;
  ospSetParam(ot, "filter", OSP_UINT, &filter);
  ospSetParam(ot, "wrapMode", OSP_UINT, &m_wrapMode);

  if (format != OSP_TEXTURE_FORMAT_INVALID && !outTransformNeedsBake(format)) {
    ospSetParam(ot, "format", OSP_UINT, &format);
    auto d = m_image->osprayData();
    ospSetParam(ot, "data", OSP_DATA, &d);
    m_unpackedColors.clear();
  } else {
    format = OSP_TEXTURE_RGBA32F;
    ospSetParam(ot, "format", OSP_UINT, &format);
    m_unpackedColors = convertToColorArray(*m_image);
    applyOutTransform(m_unpackedColors);
    auto d = ospNewSharedData1D(
        m_unpackedColors.data(), OSP_VEC4F, m_unpackedColors.size());
    ospSetParam(ot, "data", OSP_DATA, &d);
    ospRelease(d);
  }

  ospCommit(ot);
}

Attribute Image1D::inAttribute() const
{
  return m_inAttribute;
}

} // namespace anari_ospray
