// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct SpatialField : public Object
{
  SpatialField(OSPRayGlobalState *d, const char *osptype);
  ~SpatialField();
  static SpatialField *createInstance(
      std::string_view subtype, OSPRayGlobalState *d);

  OSPVolume osprayVolume() const;

 protected:
  OSPVolume m_osprayVolume{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(
    anari_ospray::SpatialField *, ANARI_SPATIAL_FIELD);
