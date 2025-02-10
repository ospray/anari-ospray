// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Volume.h"
#include "array/Array1D.h"
#include "spatial_field/SpatialField.h"

namespace anari_ospray {

struct TransferFunction1DVolume : public Volume
{
  TransferFunction1DVolume(OSPRayGlobalState *d);
  ~TransferFunction1DVolume();

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<SpatialField> m_field;
  helium::ChangeObserverPtr<Array1D> m_colorData;
  helium::ChangeObserverPtr<Array1D> m_opacityData;
  float2 m_valueRange{0.f, 1.f};
  float m_densityScale{1.f};

  OSPTransferFunction m_osprayTF{nullptr};
};

} // namespace anari_ospray
