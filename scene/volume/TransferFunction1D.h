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

  void commit() override;

  bool isValid() const override;

 private:
  helium::CommitObserverPtr<SpatialField> m_field;
  helium::CommitObserverPtr<Array1D> m_colorData;
  helium::CommitObserverPtr<Array1D> m_opacityData;

  OSPTransferFunction m_osprayTF{nullptr};
};

} // namespace anari_ospray
