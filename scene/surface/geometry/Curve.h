// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"

namespace anari_ospray {

struct Curve : public Geometry
{
  Curve(OSPRayGlobalState *s);

  void commit() override;

  void setColorAttribute(Attribute attr, OSPGeometricModel om) override;
  void setTextureCoordinateAttribute(Attribute attr) override;

 private:
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  helium::ChangeObserverPtr<Array1D> m_vertexRadius;
  std::array<helium::IntrusivePtr<Array1D>, 5> m_vertexAttributes;
  float m_globalRadius{0.f};

  std::vector<float4> m_osprayVertexRadius;
  std::vector<uint32_t> m_osprayIndex;
  std::vector<float4> m_colors;
  std::vector<float2> m_texcoords;
};

} // namespace anari_ospray
