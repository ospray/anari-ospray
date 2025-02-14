// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct Light : public Object
{
  Light(OSPRayGlobalState *d, const char *osptype);
  ~Light();

  static Light *createInstance(std::string_view subtype, OSPRayGlobalState *d);

  void commitParameters() override;
  void markFinalized() override;

  OSPLight osprayLight() const;

 protected:
  bool visible{true};
  float3 color{1};
  float intensity{1};
  OSPIntensityQuantity m_quantity;

 private:
  OSPLight m_osprayLight{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Light *, ANARI_LIGHT);
