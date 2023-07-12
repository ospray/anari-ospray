// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpatialField.h"
#include "array/Array1D.h"
#include "array/ObjectArray.h"

namespace anari_ospray {

struct AMRField : public SpatialField
{
  AMRField(OSPRayGlobalState *d);
  ~AMRField();

  void commit() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<Array1D> m_cellWidth;
  helium::IntrusivePtr<Array1D> m_block_bounds;
  helium::IntrusivePtr<Array1D> m_block_level;
  helium::IntrusivePtr<ObjectArray> m_block_data;

  std::vector<OSPData> m_extracted_block_data;
  OSPData m_ospray_block_data{nullptr};
};

} // namespace anari_ospray
