// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "SpatialField.h"
#include "array/Array1D.h"

namespace anari_ospray {

struct UnstructuredField : public SpatialField
{
  UnstructuredField(OSPRayGlobalState *d);

  void commitParameters() override;
  void finalize() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<Array1D> m_vertex_position;
  helium::IntrusivePtr<Array1D> m_vertex_data;
  helium::IntrusivePtr<Array1D> m_index;
  helium::IntrusivePtr<Array1D> m_cell_index;
  helium::IntrusivePtr<Array1D> m_cell_data;
  helium::IntrusivePtr<Array1D> m_cell_type;
  bool m_indexPrefixed{false};

  bool m_hexIterative{false};
  bool m_precomputedNormals{false};
  int m_maxIteratorDepth{6};
};

} // namespace anari_ospray
