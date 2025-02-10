// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Isosurface.h"

namespace anari_ospray {

Isosurface::Isosurface(OSPRayGlobalState *s)
    : Geometry(s, "isosurface"), m_isovalueArray(this)
{}

void Isosurface::commitParameters()
{
  Geometry::commitParameters();
  m_field = getParamObject<SpatialField>("field");
  m_isovalueArray = getParamObject<Array1D>("isovalue");
  m_isovalueSet = getParam("isovalue", ANARI_FLOAT32, &m_isovalue);
}

void Isosurface::finalize()
{
  if (!m_field) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "no spatial field provided to isosurface geometry");
    return;
  }

  auto og = osprayGeometry();

  if (m_isovalueArray && m_isovalueArray->size() > 0
      && m_isovalueArray->elementType() == ANARI_FLOAT32) {
    auto iv = m_isovalueArray->osprayData();
    ospSetParam(og, "isovalue", OSP_DATA, &iv);
  } else {
    if (!m_isovalueSet) {
      m_isovalueValid = false;
      reportMessage(ANARI_SEVERITY_WARNING,
          "missing required parameter 'isovalue' on isosurface geometry");
      return;
    }
    ospSetParam(og, "isovalue", OSP_FLOAT, &m_isovalue);
  }

  m_isovalueValid = true;

  auto ov = m_field->osprayVolume();
  ospSetParam(og, "volume", OSP_VOLUME, &ov);

  ospCommit(og);
}

bool Isosurface::isValid() const
{
  return m_field && m_field->isValid() && m_isovalueValid;
}

} // namespace anari_ospray
