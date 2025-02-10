// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpatialField.h"
#include "array/Array3D.h"

namespace anari_ospray {

struct StructuredRegularField : public SpatialField
{
  StructuredRegularField(OSPRayGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<Array3D> m_data;
  float3 m_origin{0.f};
  float3 m_spacing{1.f};
};

} // namespace anari_ospray
