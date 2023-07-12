// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.h"
// subtypes
#include "Debug.h"
#include "Pathtracer.h"
#include "SciVis.h"

namespace anari_ospray {

Renderer::Renderer(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_RENDERER, s)
{
  s->objectCounts.renderers++;
  m_osprayRenderer = ospNewRenderer(osptype);
}

Renderer::~Renderer()
{
  ospRelease(m_osprayRenderer);
  deviceState()->objectCounts.renderers--;
}

void Renderer::commit()
{
  m_bgColor = getParam<float4>("background", float4(float3(0.f), 1.f));
  m_ambientRadiance = getParam<float>("ambientRadiance", 0.f);
  m_ambientColor = getParam<float3>("ambientColor", float3(1, 1, 1));
  auto pixelSamples = getParam<int>("pixelSamples", 1);
  auto maxPathLength = getParam<int>("maxPathLength", 20);
  auto minContribution = getParam<float>("minContribution", 0.001f);
  auto varianceThreshold = getParam<float>("varianceThreshold", 0.f);

  auto r = osprayRenderer();
  ospSetParam(r, "backgroundColor", OSP_VEC4F, &m_bgColor);
  ospSetInt(r, "pixelSamples", pixelSamples);
  ospSetInt(r, "maxPathLength", maxPathLength);
  ospSetFloat(r, "minContribution", minContribution);
  ospSetFloat(r, "varianceThreshold", varianceThreshold);
}

Renderer *Renderer::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "pathtracer" || subtype == "default")
    return new Pathtracer(s);
  else if (subtype == "scivis")
    return new SciVis(s);
  else if (subtype == "debug")
    return new Debug(s);
  else
    return (Renderer *)new UnknownObject(ANARI_RENDERER, s);
}

OSPRenderer Renderer::osprayRenderer() const
{
  return m_osprayRenderer;
}

float Renderer::ambientRadiance() const
{
  return m_ambientRadiance;
}

float3 Renderer::ambientColor() const
{
  return m_ambientColor;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Renderer *);
