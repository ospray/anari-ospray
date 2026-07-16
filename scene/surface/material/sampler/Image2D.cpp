// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Image2D.h"
#include "enumCast.hpp"
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
  m_wrapMode[0] = wrapModeFromString(getParamString("wrapMode1", "clampToEdge"));
  m_wrapMode[1] = wrapModeFromString(getParamString("wrapMode2", "clampToEdge"));
}

void Image2D::finalize()
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
  ospSetParam(ot, "wrapMode", OSP_VEC2UI, m_wrapMode);

  auto size = m_image->size();
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
    auto d =
        ospNewSharedData2D(m_unpackedColors.data(), OSP_VEC4F, size.x, size.y);
    ospSetParam(ot, "data", OSP_DATA, &d);
    ospRelease(d);
  }

  ospCommit(ot);
}

Attribute Image2D::inAttribute() const
{
  return m_inAttribute;
}

} // namespace anari_ospray
