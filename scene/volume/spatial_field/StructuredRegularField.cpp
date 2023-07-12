// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "StructuredRegularField.h"
// std
#include <limits>

namespace anari_ospray {

StructuredRegularField::StructuredRegularField(OSPRayGlobalState *d)
    : SpatialField(d, "structuredRegular")
{}

void StructuredRegularField::commit()
{
  m_data = getParamObject<Array3D>("data");
  if (!m_data) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'data' on 'structuredRegular' field");
    return;
  }

  auto origin = getParam<float3>("origin", float3(0.f));
  auto spacing = getParam<float3>("spacing", float3(1.f));

  auto ov = osprayVolume();
  ospSetParam(ov, "gridOrigin", OSP_VEC3F, &origin);
  ospSetParam(ov, "gridSpacing", OSP_VEC3F, &spacing);
  auto od = m_data->osprayData();
  ospSetParam(ov, "data", OSP_DATA, &od);
  auto filter = OSP_VOLUME_FILTER_TRILINEAR;
  ospSetParam(ov, "filter", OSP_INT, &filter);
  ospCommit(ov);
}

bool StructuredRegularField::isValid() const
{
  return m_data;
}

} // namespace anari_ospray
