// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct Renderer : public Object
{
  Renderer(OSPRayGlobalState *s, const char *osptype);
  ~Renderer() override;

  virtual void commit() override;

  static Renderer *createInstance(
      std::string_view subtype, OSPRayGlobalState *d);

  OSPRenderer osprayRenderer() const;

  float ambientRadiance() const;
  float3 ambientColor() const;

 private:
  float4 m_bgColor{float3(0.f), 1.f};
  float m_ambientRadiance{0.f};
  float3 m_ambientColor{1.f, 1.f, 1.f};

  OSPRenderer m_osprayRenderer{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Renderer *, ANARI_RENDERER);
