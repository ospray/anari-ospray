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

AMRField::AMRField(OSPRayGlobalState *d)
    : SpatialField(d, "amr"),
      m_refinementRatio(this),
      m_block_start(this),
      m_block_data(this)
{}

void AMRField::commitParameters()
{
  m_refinementRatio = getParamObject<Array1D>("refinementRatio");
  m_block_start = getParamObject<Array1D>("block.start");
  m_block_level = getParamObject<Array1D>("block.level");
  m_block_data = getParamObject<ObjectArray>("block.data");

  if (!m_block_data) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'block.data' on 'amr' field");
    return;
  }

  if (m_block_data->elementType() != ANARI_ARRAY3D) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'block.data' on 'amr' field must be ANARIArray3D");
    return;
  }

  if (!m_block_start) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'block.start' on 'amr' field");
    return;
  }

  if (!m_block_level) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'block.level' on 'amr' field");
    return;
  }

  if (!m_refinementRatio) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'refinementRatio' on 'amr' field");
    return;
  }

  if (m_refinementRatio->elementType() != ANARI_UINT32) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'refinementRatio' on 'amr' field must be array of UINT32");
    return;
  }

  if (m_block_data->size() != m_block_start->size()
      || m_block_data->size() != m_block_level->size()) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "'block.data', 'block.start', and 'block.level' must have the same "
        "number of elements");
    return;
  }

  m_origin = getParam<float3>("origin", float3(0.f));
  m_spacing = getParam<float3>("spacing", float3(1.f));
  m_method = amrMethodFromString(getParamString("method", "current"));
}

void AMRField::finalize()
{
  if (!isValid())
    return;

  std::vector<OSPData> extracted_block_data;
  std::vector<std::pair<int3, int3>> block_bounds;

  int i = 0;
  std::for_each(
      m_block_data->handlesBegin(), m_block_data->handlesEnd(), [&](Object *o) {
        auto *a = (Array3D *)o;
        extracted_block_data.push_back(a->osprayData());
        const int3 start = m_block_start->dataAs<int3>()[i];
        block_bounds.push_back(
            std::make_pair(start, start + (int3)a->size() - 1));
        i++;
      });

  auto ospray_block_data = ospNewSharedData(
      extracted_block_data.data(), OSP_DATA, extracted_block_data.size());

  for (auto &o : extracted_block_data)
    ospRelease(o);

  std::vector<float> cellWidth;
  cellWidth.push_back(m_spacing.x);
  std::for_each(m_refinementRatio->beginAs<uint32_t>(),
      m_refinementRatio->endAs<uint32_t>(),
      [&](uint32_t r) { cellWidth.push_back(cellWidth.back() * r); });

  auto ov = osprayVolume();
  ospSetParam(ov, "gridOrigin", OSP_VEC3F, &m_origin);
  ospSetParam(ov, "gridSpacing", OSP_VEC3F, &m_spacing);

  auto ocw = ospNewSharedData(cellWidth.data(), OSP_FLOAT, cellWidth.size());
  ospSetParam(ov, "cellWidth", OSP_DATA, &ocw);
  ospRelease(ocw);

  auto obb =
      ospNewSharedData(block_bounds.data(), OSP_BOX3I, block_bounds.size());
  ospSetParam(ov, "block.bounds", OSP_DATA, &obb);
  ospRelease(obb);

  auto obl = m_block_level->osprayData();
  ospSetParam(ov, "block.level", OSP_DATA, &obl);

  ospSetParam(ov, "block.data", OSP_DATA, &ospray_block_data);
  ospRelease(ospray_block_data);
  ospCommit(ov);

  m_extracted_block_data = std::move(extracted_block_data);
  m_block_bounds = std::move(block_bounds);
  m_cellWidth = std::move(cellWidth);
}

bool AMRField::isValid() const
{
  return m_block_data && m_block_start && m_block_level && m_refinementRatio
      && m_block_data->size() == m_block_start->size()
      && m_block_data->size() == m_block_level->size();
}

} // namespace anari_ospray
