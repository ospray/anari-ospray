// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Cylinder.h"
// std
#include <algorithm>
#include <numeric>

namespace anari_ospray {

Cylinder::Cylinder(OSPRayGlobalState *s)
    : Geometry(s, "curve"),
      m_index(this),
      m_radius(this),
      m_vertexPosition(this)
{}

void Cylinder::commitParameters()
{
  Geometry::commitParameters();
  m_index = getParamObject<Array1D>("primitive.index");
  m_radius = getParamObject<Array1D>("primitive.radius");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
  m_globalRadius = getParam<float>("radius", 1.f);
}

void Cylinder::finalize()
{
  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on cylinder geometry");
    return;
  }

  const float *radius = m_radius ? m_radius->beginAs<float>() : nullptr;

  std::vector<float4> osprayVertexRadius;
  std::vector<uint32_t> osprayIndex;

  {
    osprayVertexRadius.resize(m_vertexPosition->size());
    uint32_t rID = 0;
    std::transform(m_vertexPosition->beginAs<float3>(),
        m_vertexPosition->endAs<float3>(),
        osprayVertexRadius.begin(),
        [&](const float3 &v) {
          return float4(v, radius ? radius[rID++ / 2] : m_globalRadius);
        });
  }

  auto og = osprayGeometry();

  // Remove old parameters //

  ospRemoveParam(og, "vertex.position_radius");
  ospRemoveParam(og, "index");

  // Set new parameters //

  auto vrd = ospNewSharedData(
      osprayVertexRadius.data(), OSP_VEC4F, osprayVertexRadius.size());
  ospSetParam(og, "vertex.position_radius", OSP_DATA, &vrd);
  ospRelease(vrd);

  if (m_index) {
    reportMessage(ANARI_SEVERITY_WARNING, "making indexed cylinders");
    auto id = m_index->osprayData();
    ospSetParam(og, "index", OSP_DATA, &id);
  } else {
    reportMessage(ANARI_SEVERITY_WARNING, "making cylinder soup");
    osprayIndex.resize(m_vertexPosition->size() / 2);
    std::iota(osprayIndex.begin(), osprayIndex.end(), 0);
    std::transform(osprayIndex.begin(),
        osprayIndex.end(),
        osprayIndex.begin(),
        [](auto &i) { return i * 2; });
    auto id =
        ospNewSharedData(osprayIndex.data(), OSP_UINT, osprayIndex.size());
    ospSetParam(og, "index", OSP_DATA, &id);
    ospRelease(id);
  }

  auto curveType = OSP_DISJOINT;
  auto curveBasis = OSP_LINEAR;

  ospSetParam(og, "type", OSP_UINT, &curveType);
  ospSetParam(og, "basis", OSP_UINT, &curveBasis);

  ospCommit(og);

  m_osprayVertexRadius = std::move(osprayVertexRadius);
  m_osprayIndex = std::move(osprayIndex);
}

void Cylinder::setColorAttribute(Attribute attr, OSPGeometricModel om)
{
  Geometry::setColorAttribute(attr, om);

  std::vector<float4> unpackedValues;

  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.color");

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];

    if (va)
      unpackedValues = convertToColorArray(*va);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC4F, unpackedValues.size());
      ospSetParam(og, "vertex.color", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_colors = std::move(unpackedValues);
}

void Cylinder::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];

    if (va)
      unpackedValues = convertToTexcoordArray(*va);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC2F, unpackedValues.size());
      ospSetParam(og, "vertex.texcoord", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_texcoords = std::move(unpackedValues);
}

} // namespace anari_ospray
