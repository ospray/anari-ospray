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

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

 private:
  helium::ChangeObserverPtr<Array1D> m_refinementRatio;
  helium::ChangeObserverPtr<Array1D> m_block_start;
  helium::IntrusivePtr<Array1D> m_block_level;
  helium::ChangeObserverPtr<ObjectArray> m_block_data;

  float3 m_origin{0.f};
  float3 m_spacing{1.f};
  OSPAMRMethod m_method{OSP_AMR_CURRENT};

  std::vector<OSPData> m_extracted_block_data;
  std::vector<std::pair<int3, int3>> m_block_bounds;
  std::vector<float> m_cellWidth;
};

} // namespace anari_ospray
