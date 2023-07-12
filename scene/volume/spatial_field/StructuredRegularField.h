// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpatialField.h"
#include "array/Array3D.h"

namespace anari_ospray {

struct StructuredRegularField : public SpatialField
{
  StructuredRegularField(OSPRayGlobalState *d);

  void commit() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<Array3D> m_data;
};

} // namespace anari_ospray
