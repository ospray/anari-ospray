// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"

namespace anari_ospray {

// Unified mapping of the ANARI cone/curve/cylinder geometries onto OSPRay's
// 'curve' geometry (they only differ in a few config knobs).
struct Curves : public Geometry
{
  enum class Subtype
  {
    CONE,
    CURVE,
    CYLINDER
  };

  Curves(OSPRayGlobalState *s, Subtype subtype);

  void commitParameters() override;
  void finalize() override;

  void setColorAttribute(Attribute attr, OSPGeometricModel om) override;
  void setTextureCoordinateAttribute(Attribute attr) override;

 private:
  const Subtype m_subtype;
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  helium::ChangeObserverPtr<Array1D> m_radius;
  std::array<helium::ChangeObserverPtr<Array1D>, 5> m_vertexAttributes;
  float m_globalRadius{1.f};

  std::vector<float4> m_osprayVertexRadius;
  std::vector<uint32_t> m_osprayIndex;
  // OSPRay vertex -> source ANARI vertex, for expanding per-vertex attributes.
  // Empty unless the vertex pool was expanded.
  std::vector<uint32_t> m_vertexRemap;
  std::vector<float4> m_colors;
  std::vector<float2> m_texcoords;
};

} // namespace anari_ospray
