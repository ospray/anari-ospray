// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct Renderer : public Object
{
  Renderer(OSPRayGlobalState *s, const char *osptype);
  ~Renderer() override;

  virtual void commitParameters() override;
  virtual void finalize() override;

  static Renderer *createInstance(
      std::string_view subtype, OSPRayGlobalState *d);

  OSPRenderer osprayRenderer() const;

  float ambientRadiance() const;
  float3 ambientColor() const;

  bool denoise() const;
  bool denoiseAlpha() const;
  OSPDenoiserQuality denoiseQuality() const;

 private:
  float4 m_bgColor{float3(0.f), 1.f};
  float m_ambientRadiance{0.f};
  float3 m_ambientColor{1.f, 1.f, 1.f};
  int m_pixelSamples{1};
  int m_maxPathLength{20};
  float m_minContribution{0.001f};
  float m_varianceThreshold{0.f};
  bool m_denoiseEnabled{false};
  bool m_denoiseAlpha{false};
  std::string m_denoiseQualityString;
  OSPDenoiserQuality m_denoiseQuality{OSP_DENOISER_QUALITY_MEDIUM};

  OSPRenderer m_osprayRenderer{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Renderer *, ANARI_RENDERER);
