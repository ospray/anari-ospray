// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "StructuredRegularField.h"
// std
#include <limits>

namespace anari_ospray {

// Helper functions ///////////////////////////////////////////////////////////

static OSPVolumeFilter filterFromString(const std::string &str)
{
  if (str == "nearest")
    return OSP_VOLUME_FILTER_NEAREST;
  else if (str == "cubic")
    return OSP_VOLUME_FILTER_CUBIC;
  else
    return OSP_VOLUME_FILTER_LINEAR;
}

// StructuredRegularField definitions //////////////////////////////////////////

StructuredRegularField::StructuredRegularField(OSPRayGlobalState *d)
    : SpatialField(d, "structuredRegular"), m_data(this)
{}

void StructuredRegularField::commitParameters()
{
  m_data = getParamObject<Array3D>("data");
  m_origin = getParam<float3>("origin", float3(0.f));
  m_spacing = getParam<float3>("spacing", float3(1.f));
  m_filter = filterFromString(getParamString("filter", "linear"));
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
  auto filter = m_filter;
  ospSetParam(ov, "filter", OSP_UINT, &filter);
  ospCommit(ov);
}

bool StructuredRegularField::isValid() const
{
  return m_data;
}

ANARIDataType StructuredRegularField::elementType() const
{
  return m_data ? m_data->elementType() : ANARI_UNKNOWN;
}

} // namespace anari_ospray
