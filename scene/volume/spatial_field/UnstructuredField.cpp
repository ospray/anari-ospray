// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "UnstructuredField.h"

namespace anari_ospray {

// UnstructuredField definitions //////////////////////////////////////////////

UnstructuredField::UnstructuredField(OSPRayGlobalState *d)
  : SpatialField(d, "unstructured") {}

void UnstructuredField::commit()
{
  m_vertex_position = getParamObject<Array1D>("vertex.position");
  m_vertex_data = getParamObject<Array1D>("vertex.data");
  m_index = getParamObject<Array1D>("index");
  m_cell_index = getParamObject<Array1D>("cell.index");
  m_cell_data = getParamObject<Array1D>("cell.data");
  m_cell_type = getParamObject<Array1D>("cell.type");
  if (!m_vertex_position) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing 'vertex.position' on 'unstructured' field");
    return;
  }

  if (!m_index) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing 'index' on 'unstructured' field");
    return;
  }

  if (!m_vertex_data && !m_cell_data) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing data ('vertex.data' or 'cell.data') on 'unstructured' field");
    return;
  }

  m_indexPrefixed = getParam<bool>("indexPrefixed", false);
  auto hexIterative = getParam<bool>("hexIterative", false);
  auto precomputedNormals = getParam<bool>("precomputedNormals", false);
  auto maxIteratorDepth = getParam<int>("maxIteratorDepth", 6);

  auto ov = osprayVolume();
  auto ovp = m_vertex_position->osprayData();
  ospSetParam(ov, "vertex.position", OSP_DATA, &ovp);
  if (m_vertex_data) {
    auto ovd = m_vertex_data->osprayData();
    ospSetParam(ov, "vertex.data", OSP_DATA, &ovd);
  }
  auto oi = m_index->osprayData();
  ospSetParam(ov, "index", OSP_DATA, &oi);
  if (m_cell_index) {
    auto oci = m_cell_index->osprayData();
    ospSetParam(ov, "cell.index", OSP_DATA, &oci);
  }
  if (m_cell_data) {
    auto ocd = m_cell_data->osprayData();
    ospSetParam(ov, "cell.data", OSP_DATA, &ocd);
  }
  if (m_cell_type) {
    auto oct = m_cell_type->osprayData();
    ospSetParam(ov, "cell.type", OSP_DATA, &oct);
  }
  ospSetParam(ov, "indexPrefixed", OSP_BOOL, &m_indexPrefixed);
  ospSetParam(ov, "hexIterative", OSP_BOOL, &hexIterative);
  ospSetParam(ov, "precomputedNormals", OSP_BOOL, &precomputedNormals);
  ospSetParam(ov, "maxIteratorDepth", OSP_INT, &maxIteratorDepth);
  ospCommit(ov);
}

bool UnstructuredField::isValid() const
{
  bool haveData = (m_vertex_data || m_cell_data);
  if (m_indexPrefixed)
    return haveData && m_vertex_position && m_index;
  else
    return haveData && m_vertex_position && m_index && m_cell_index && m_cell_type;
}

} // namespace anari_ospray
