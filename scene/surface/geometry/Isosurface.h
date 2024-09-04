// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "scene/volume/spatial_field/SpatialField.h"

namespace anari_ospray {

struct Isosurface : public Geometry
{
  Isosurface(OSPRayGlobalState *s);

  void commit() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<SpatialField> m_field;
  helium::ChangeObserverPtr<Array1D> m_isovalue;
};

} // namespace anari_ospray
