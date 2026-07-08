// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Renderer.h"
// subtypes
#include "Debug.h"
#include "Pathtracer.h"
#include "SciVis.h"

namespace anari_ospray {

Renderer::Renderer(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_RENDERER, s), m_bgImage(this)
{
  m_osprayRenderer = ospNewRenderer(osptype);
}

Renderer::~Renderer()
{
  ospRelease(m_backplateTexture);
  ospRelease(m_osprayRenderer);
}

void Renderer::commitParameters()
{
  m_bgColor = getParam<float4>("background", float4(float3(0.f), 1.f));
  m_bgImage = getParamObject<Array2D>("background");
  m_ambientRadiance = getParam<float>("ambientRadiance", 0.f);
  m_ambientColor = getParam<float3>("ambientColor", float3(1, 1, 1));
  m_pixelSamples = getParam<int>("pixelSamples", 1);
  m_maxPathLength = getParam<int>("maxPathLength", 20);
  m_minContribution = getParam<float>("minContribution", 0.001f);
  m_varianceThreshold = getParam<float>("varianceThreshold", 0.f);
  m_denoiseEnabled = getParam<bool>("denoise", false);
  m_denoiseAlpha = getParam<bool>("denoiseAlpha", false);
  m_denoiseQualityString = getParamString("denoiseQuality", "medium");
}

void Renderer::finalize()
{
  auto r = osprayRenderer();
  if (m_bgImage) {
    if (!m_backplateTexture)
      m_backplateTexture = ospNewTexture("texture2d");

    m_bgImageColors = convertToColorArray(*m_bgImage);
    uint32_t format = OSP_TEXTURE_RGBA32F;
    ospSetParam(m_backplateTexture, "format", OSP_UINT, &format);
    const auto size = m_bgImage->size();
    auto d = ospNewSharedData2D(
        m_bgImageColors.data(), OSP_VEC4F, size.x, size.y);
    ospSetParam(m_backplateTexture, "data", OSP_DATA, &d);
    ospRelease(d);
    ospCommit(m_backplateTexture);
    ospSetObject(r, "map_backplate", m_backplateTexture);
    ospRemoveParam(r, "backgroundColor");
  } else {
    ospSetParam(r, "backgroundColor", OSP_VEC4F, &m_bgColor);
    ospRemoveParam(r, "map_backplate");
  }
  ospSetInt(r, "pixelSamples", m_pixelSamples);
  ospSetInt(r, "maxPathLength", m_maxPathLength);
  ospSetFloat(r, "minContribution", m_minContribution);
  ospSetFloat(r, "varianceThreshold", m_varianceThreshold);

  if (m_denoiseQualityString == "low")
    m_denoiseQuality = OSP_DENOISER_QUALITY_LOW;
  else if (m_denoiseQualityString == "high")
    m_denoiseQuality = OSP_DENOISER_QUALITY_HIGH;
  else {
    m_denoiseQuality = OSP_DENOISER_QUALITY_MEDIUM;
    if (m_denoiseQualityString.empty())
      reportMessage(ANARI_SEVERITY_WARNING,
          "Parameter denoiseQuality is empty. Acceptable values are: 'low', 'medium', 'high'.");
    else if (m_denoiseQualityString != "medium")
      reportMessage(ANARI_SEVERITY_WARNING,
          "Invalid value for denoiseQuality: '%s'. Acceptable values are: 'low', 'medium', 'high'.",
          m_denoiseQualityString);
  }

  if (m_denoiseEnabled && ospLoadModule("denoiser") != OSP_NO_ERROR) {
    m_denoiseEnabled = false;
    reportMessage(
        ANARI_SEVERITY_WARNING, "denoiser requested but not available");
  }
}

Renderer *Renderer::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "pathtracer" || (!s->distributed && subtype == "default"))
    return new Pathtracer(s);
  else if (subtype == "scivis")
    return new SciVis(s);
  else if (subtype == "mpiRaycast" || (s->distributed && subtype == "default"))
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

bool Renderer::denoise() const
{
  return m_denoiseEnabled;
}

bool Renderer::denoiseAlpha() const
{
  return m_denoiseAlpha;
}

OSPDenoiserQuality Renderer::denoiseQuality() const
{
  return m_denoiseQuality;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Renderer *);
