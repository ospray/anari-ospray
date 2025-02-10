// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "StructuredRegularField.h"
// std
#include <limits>

namespace anari_ospray {

StructuredRegularField::StructuredRegularField(OSPRayGlobalState *d)
    : SpatialField(d, "structuredRegular")
{}

void StructuredRegularField::commitParameters()
{
  m_data = getParamObject<Array3D>("data");
  m_origin = getParam<float3>("origin", float3(0.f));
  m_spacing = getParam<float3>("spacing", float3(1.f));
}

void StructuredRegularField::finalize()
{
  if (!m_data) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'data' on 'structuredRegular' field");
    return;
  }

  auto ov = osprayVolume();
  ospSetParam(ov, "gridOrigin", OSP_VEC3F, &m_origin);
  ospSetParam(ov, "gridSpacing", OSP_VEC3F, &m_spacing);
  auto od = m_data->osprayData();
  ospSetParam(ov, "data", OSP_DATA, &od);
  auto filter = OSP_VOLUME_FILTER_LINEAR;
  ospSetParam(ov, "filter", OSP_UINT, &filter);
  ospCommit(ov);
}

bool StructuredRegularField::isValid() const
{
  return m_data;
}

} // namespace anari_ospray
