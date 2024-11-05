// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Isosurface.h"

namespace anari_ospray {

Isosurface::Isosurface(OSPRayGlobalState *s)
    : Geometry(s, "isosurface"), m_isovalue(this)
{}

void Isosurface::commit()
{
  Geometry::commit();

  m_isovalueValid = false;

  m_field = getParamObject<SpatialField>("field");

  if (!m_field) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "no spatial field provided to isosurface geometry");
    return;
  }

  m_isovalue = getParamObject<Array1D>("isovalue");

  auto og = osprayGeometry();

  if (m_isovalue && m_isovalue->size() > 0
      && m_isovalue->elementType() == ANARI_FLOAT32) {
    auto iv = m_isovalue->osprayData();
    ospSetParam(og, "isovalue", OSP_DATA, &iv);
  } else {
    float isovalue;
    if (!getParam("isovalue", ANARI_FLOAT32, &isovalue)) {
      reportMessage(ANARI_SEVERITY_WARNING,
          "missing required parameter 'isovalue' on isosurface geometry");
      return;
    }
    ospSetParam(og, "isovalue", OSP_FLOAT, &isovalue);
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
