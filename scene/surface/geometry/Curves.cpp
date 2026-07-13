// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Curves.h"
// std
#include <algorithm>
#include <numeric>

namespace anari_ospray {

Curves::Curves(OSPRayGlobalState *s, Subtype subtype)
    : Geometry(s, "curve"),
      m_subtype(subtype),
      m_index(this),
      m_vertexPosition(this),
      m_radius(this),
      m_vertexAttributes{this, this, this, this, this}
{}

void Curves::commitParameters()
{
  Geometry::commitParameters();
  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_radius = getParamObject<Array1D>(
      m_subtype == Subtype::CYLINDER ? "primitive.radius" : "vertex.radius");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
  m_globalRadius = getParam<float>("radius", 1.f);
}

void Curves::finalize()
{
  if (!m_vertexPosition || m_vertexPosition->size() < 2) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position'"
        " with at least 2 vertices on cone/curve/cylinder geometry");
    return;
  }

  const float3 *positions = m_vertexPosition->beginAs<float3>();
  const float *radius = m_radius ? m_radius->beginAs<float>() : nullptr;

  std::vector<float4> osprayVertexRadius;
  std::vector<uint32_t> osprayIndex;
  std::vector<uint32_t> vertexRemap;

  const size_t nv = m_vertexPosition->size();
  const float sentinel = -1.0f;
  osprayVertexRadius.resize(nv);
  for (size_t v = 0; v < nv; ++v) {
    float r = m_globalRadius;
    if (radius)
      r = m_subtype != Subtype::CYLINDER ? radius[v] : sentinel;
    osprayVertexRadius[v] = float4(positions[v], r);
  }

  if (m_subtype == Subtype::CURVE) {
    if (m_index)
      osprayIndex.clear();
    else {
      osprayIndex.resize(nv - 1);
      std::iota(osprayIndex.begin(), osprayIndex.end(), 0);
    }
  } else if (m_index) {
    // cone/cylinder: each index is a (start, end) vertex pair
    const size_t np = m_index->size();
    const uint2 *pairs = m_index->beginAs<uint2>();

    bool expand = false;
    // check if any of the pairs are non-consecutive
    for (size_t p = 0; p < np; ++p)
      if (pairs[p].y != pairs[p].x + 1) {
        expand = true;
        break;
      }

    if (!expand && radius && m_subtype == Subtype::CYLINDER) {
      // check if any of the pairs have different radii
      auto sharedVertexConflict = [&](size_t idx, float radius) {
        if (osprayVertexRadius[idx].w != sentinel)
          return osprayVertexRadius[idx].w != radius;
        osprayVertexRadius[idx].w = radius;
        return false;
      };
      for (size_t p = 0; p < np; ++p)
        if (sharedVertexConflict(pairs[p].x, radius[p])
            || sharedVertexConflict(pairs[p].y, radius[p])) {
          expand = true;
          break;
        }
    }

    if (expand) { // compact two-vertices-per-primitive buffer
      osprayVertexRadius.resize(np * 2);
      osprayIndex.resize(np);
      vertexRemap.resize(np * 2);
      for (size_t p = 0; p < np; ++p) {
        float r = m_globalRadius;
        if (radius)
          r = m_subtype == Subtype::CYLINDER ? radius[p] : radius[pairs[p].x];
        osprayVertexRadius[p * 2] = float4(positions[pairs[p].x], r);
        if (radius && m_subtype != Subtype::CYLINDER)
          r = radius[pairs[p].y];
        osprayVertexRadius[p * 2 + 1] = float4(positions[pairs[p].y], r);
        osprayIndex[p] = p * 2;
        vertexRemap[p * 2] = pairs[p].x;
        vertexRemap[p * 2 + 1] = pairs[p].y;
      }
    } else { // reuse the original vertex pool, base index = x
      osprayIndex.resize(np);
      for (size_t p = 0; p < np; ++p)
        osprayIndex[p] = pairs[p].x;
    }
  } else {
    // cone/cylinder soup: consecutive vertex pairs form the primitives
    osprayIndex.resize(nv / 2);
    for (size_t p = 0; p < osprayIndex.size(); ++p) {
      osprayIndex[p] = p * 2;
      if (radius && m_subtype == Subtype::CYLINDER) {
        const float r = radius[p];
        osprayVertexRadius[p * 2].w = r;
        osprayVertexRadius[p * 2 + 1].w = r;
      }
    }
  }

  auto og = osprayGeometry();

  // Remove old parameters //

  ospRemoveParam(og, "vertex.position_radius");
  ospRemoveParam(og, "index");

  // Set new parameters //

  const auto vrd = ospNewSharedData(
      osprayVertexRadius.data(), OSP_VEC4F, osprayVertexRadius.size());
  ospSetParam(og, "vertex.position_radius", OSP_DATA, &vrd);
  ospRelease(vrd);

  if (m_index && m_subtype == Subtype::CURVE) {
    const auto id = m_index->osprayData();
    ospSetParam(og, "index", OSP_DATA, &id);
  } else {
    const auto id = ospNewSharedData(osprayIndex.data(), OSP_UINT, osprayIndex.size());
    ospSetParam(og, "index", OSP_DATA, &id);
    ospRelease(id);
  }

  const auto curveType =
      m_subtype == Subtype::CURVE ? OSP_ROUND : OSP_DISJOINT;
  const auto curveBasis = OSP_LINEAR;
  ospSetParam(og, "type", OSP_UINT, &curveType);
  ospSetParam(og, "basis", OSP_UINT, &curveBasis);

  ospCommit(og);

  m_osprayVertexRadius = std::move(osprayVertexRadius);
  m_osprayIndex = std::move(osprayIndex);
  m_vertexRemap = std::move(vertexRemap);
}

void Curves::setColorAttribute(Attribute attr, OSPGeometricModel om)
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

    if (!unpackedValues.empty() && !m_vertexRemap.empty()) {
      std::vector<float4> expanded(m_vertexRemap.size());
      for (size_t i = 0; i < m_vertexRemap.size(); ++i)
        expanded[i] = unpackedValues[m_vertexRemap[i]];
      unpackedValues = std::move(expanded);
    }

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

void Curves::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];

    if (va)
      unpackedValues = convertToTexcoordArray(*va);

    if (!unpackedValues.empty() && !m_vertexRemap.empty()) {
      std::vector<float2> expanded(m_vertexRemap.size());
      for (size_t i = 0; i < m_vertexRemap.size(); ++i)
        expanded[i] = unpackedValues[m_vertexRemap[i]];
      unpackedValues = std::move(expanded);
    }

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
