// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"
#include "scene/volume/spatial_field/SpatialField.h"

namespace anari_ospray {

struct Isosurface : public Geometry
{
  Isosurface(OSPRayGlobalState *s);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<SpatialField> m_field;
  helium::ChangeObserverPtr<Array1D> m_isovalueArray;

  float m_isovalue{0.f};
  bool m_isovalueSet{false};

  // Either scalar or array provided:
  bool m_isovalueValid{false};
};

} // namespace anari_ospray
