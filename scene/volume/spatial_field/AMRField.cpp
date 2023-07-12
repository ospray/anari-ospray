// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "AMRField.h"
#include "array/Array3D.h"

namespace anari_ospray {

// Helper functions ///////////////////////////////////////////////////////////

static OSPAMRMethod amrMethodFromString(const std::string &str)
{
  if (str == "octant")
    return OSP_AMR_OCTANT;
  else if (str == "finest")
    return OSP_AMR_FINEST;
  else
    return OSP_AMR_CURRENT;
}

// AMRField definitions ///////////////////////////////////////////////////////

AMRField::AMRField(OSPRayGlobalState *d) : SpatialField(d, "amr") {}

AMRField::~AMRField()
{
#if 0
  for (auto &o : m_extracted_block_data)
    ospRelease(o);
  ospRelease(m_ospray_block_data);
#endif
}

void AMRField::commit()
{
  m_cellWidth = getParamObject<Array1D>("cellWidth");
  m_block_bounds = getParamObject<Array1D>("block.bounds");
  m_block_level = getParamObject<Array1D>("block.level");
  m_block_data = getParamObject<ObjectArray>("block.data");
  if (!m_block_data) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'block.data' on 'amr' field");
    return;
  }

  if (m_block_data->elementType() != ANARI_ARRAY3D) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'block.data' on 'amr' field isn't ANARIArray3D");
  }

  if (m_block_data->elementType() == ANARI_ARRAY1D) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "allowing 'block.data' to be ANARIArray1D on 'amr' field");
  } else {
    return;
  }

  auto origin = getParam<float3>("origin", float3(0.f));
  auto spacing = getParam<float3>("spacing", float3(1.f));
  auto method = amrMethodFromString(getParamString("method", "current"));

  std::vector<OSPData> extracted_block_data;

  std::for_each(
      m_block_data->handlesBegin(), m_block_data->handlesEnd(), [&](Object *o) {
        auto *a = (Array3D *)o;
        extracted_block_data.push_back(a->osprayData());
      });

  auto ospray_block_data = ospNewSharedData(
      extracted_block_data.data(), OSP_DATA, extracted_block_data.size());

  auto ov = osprayVolume();
  ospSetParam(ov, "gridOrigin", OSP_VEC3F, &origin);
  ospSetParam(ov, "gridSpacing", OSP_VEC3F, &spacing);
  auto ocw = m_cellWidth->osprayData();
  ospSetParam(ov, "cellWidth", OSP_DATA, &ocw);
  auto obb = m_block_bounds->osprayData();
  ospSetParam(ov, "block.bounds", OSP_DATA, &obb);
  auto obl = m_block_level->osprayData();
  ospSetParam(ov, "block.level", OSP_DATA, &obl);
  ospSetParam(ov, "block.data", OSP_DATA, &ospray_block_data);
  ospCommit(ov);

  for (auto &o : m_extracted_block_data)
    ospRelease(o);
  ospRelease(m_ospray_block_data);
  m_extracted_block_data = std::move(extracted_block_data);
  m_ospray_block_data = ospray_block_data;
}

bool AMRField::isValid() const
{
  return m_block_data && m_block_bounds && m_block_level && m_cellWidth;
}

} // namespace anari_ospray
