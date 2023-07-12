// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct Geometry;

struct Sampler : public Object
{
  Sampler(OSPRayGlobalState *d);
  ~Sampler();

  static Sampler *createInstance(
      std::string_view subtype, OSPRayGlobalState *d);

  OSPTexture osprayTexture() const;
  virtual Attribute inAttribute() const = 0;

 protected:
  OSPTexture m_osprayTexture{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Sampler *, ANARI_SAMPLER);
