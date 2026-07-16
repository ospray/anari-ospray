// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"
// subtypes
#include "Image1D.h"
#include "Image2D.h"

namespace anari_ospray {

Sampler::Sampler(OSPRayGlobalState *s) : Object(ANARI_SAMPLER, s) {}

Sampler::~Sampler()
{
  ospRelease(m_osprayTexture);
}

void Sampler::commitParameters()
{
  const mat4 identity(linalg::identity);
  m_inTransform = getParam<mat4>("inTransform", identity);
  m_inOffset = getParam<float4>("inOffset", float4(0.f));
  m_outTransform = getParam<mat4>("outTransform", identity);
  m_outOffset = getParam<float4>("outOffset", float4(0.f));
  m_hasInTransform = m_inTransform != identity || m_inOffset != float4(0.f);
  m_hasOutTransform = m_outTransform != identity || m_outOffset != float4(0.f);
}

uint32_t Sampler::wrapModeFromString(const std::string &str)
{
  if (str == "repeat")
    return OSP_TEXTURE_WRAP_REPEAT;
  else if (str == "mirrorRepeat")
    return OSP_TEXTURE_WRAP_MIRRORED_REPEAT;
  else
    return OSP_TEXTURE_WRAP_CLAMP_TO_EDGE;
}

void Sampler::applyInTransform(OSPMaterial om, const char *mapName) const
{
  const auto transformName = std::string(mapName) + ".transform";
  const auto translationName = std::string(mapName) + ".translation";

  if (!m_hasInTransform) {
    ospRemoveParam(om, transformName.c_str());
    ospRemoveParam(om, translationName.c_str());
    return;
  }

  // Reduce 4x4 on (u,v,0,1); OSPRay negates placement translations.
  const float linear2f[4] = {m_inTransform.x.x,
      m_inTransform.x.y,
      m_inTransform.y.x,
      m_inTransform.y.y};
  const float2 translation = {
      -m_inTransform.w.x - m_inOffset.x,
      -m_inTransform.w.y - m_inOffset.y};

  ospSetParam(om, transformName.c_str(), OSP_LINEAR2F, linear2f);
  ospSetParam(om, translationName.c_str(), OSP_VEC2F, &translation);
}

void Sampler::applyOutTransform(std::vector<float4> &colors) const
{
  if (!m_hasOutTransform)
    return;
  for (auto &c : colors)
    c = linalg::mul(m_outTransform, c) + m_outOffset;
}

bool Sampler::outTransformNeedsBake(OSPTextureFormat format) const
{
  if (!m_hasOutTransform)
    return false;
  // L8/LA8 turn (r,0,0,w) into (r,r,r,w); a 'copy R to G,B' transform is then a no-op
  if (format == OSP_TEXTURE_L8 || format == OSP_TEXTURE_LA8) {
    const bool broadcastRToGB = m_outTransform.x == float4(1.f, 1.f, 1.f, 0.f)
        && m_outTransform.w == float4(0.f, 0.f, 0.f, 1.f)
        && m_outOffset == float4(0.f);
    if (broadcastRToGB)
      return false;
  }
  return true;
}

Sampler *Sampler::createInstance(std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "image1D")
    return new Image1D(s);
  else if (subtype == "image2D")
    return new Image2D(s);
  else
    return (Sampler *)new UnknownObject(ANARI_SAMPLER, s);
}

OSPTexture Sampler::osprayTexture() const
{
  return m_osprayTexture;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Sampler *);
