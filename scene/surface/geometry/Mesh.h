// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.h"

namespace anari_ospray {

// Unified mapping of the ANARI triangle/quad geometries onto OSPRay's 'mesh'
// geometry (they only differ in their primitive size).
struct Mesh : public Geometry
{
  enum class Subtype
  {
    TRIANGLE,
    QUAD
  };

  Mesh(OSPRayGlobalState *s, Subtype subtype);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

  void setColorAttribute(Attribute attr, OSPGeometricModel om) override;
  void setTextureCoordinateAttribute(Attribute attr) override;

 private:
  const Subtype m_subtype;
  helium::ChangeObserverPtr<Array1D> m_index;
  helium::ChangeObserverPtr<Array1D> m_vertexPosition;
  helium::ChangeObserverPtr<Array1D> m_vertexNormal;
  std::array<helium::ChangeObserverPtr<Array1D>, 5> m_vertexAttributes;
  helium::ChangeObserverPtr<Array1D> m_faceVaryingNormal;
  std::array<helium::ChangeObserverPtr<Array1D>, 5> m_faceVaryingAttributes;

  std::vector<uint32_t> m_indices;
  std::vector<float4> m_colors;
  std::vector<float2> m_texcoords;
};

} // namespace anari_ospray
