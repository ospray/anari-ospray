// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"

namespace anari_ospray {

struct Sphere : public Geometry
{
  Sphere(OSPRayGlobalState *s);

  void commit() override;

  void setTextureCoordinateAttribute(Attribute attr) override;

 private:
  helium::CommitObserverPtr<Array1D> m_index;
  helium::CommitObserverPtr<Array1D> m_vertexPosition;
  helium::CommitObserverPtr<Array1D> m_vertexRadius;
  float m_globalRadius{0.f};

  std::vector<float2> m_texcoords;
};

} // namespace anari_ospray
