// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Quad.h"
// std
#include <numeric>

namespace anari_ospray {

Quad::Quad(OSPRayGlobalState *s) : Geometry(s, "mesh"), m_index(this), m_vertexPosition(this) {}

void Quad::commit()
{
  Geometry::commit();

  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");

  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on quad geometry");
    return;
  }

  auto og = osprayGeometry();

  std::vector<uint32_t> indices;

  // Remove old parameters //

  ospRemoveParam(og, "vertex.position");
  ospRemoveParam(og, "index");

  // Set new parameters //

  auto vpd = m_vertexPosition->osprayData();
  ospSetParam(og, "vertex.position", OSP_DATA, &vpd);
  if (m_index) {
    auto id = m_index->osprayData();
    ospSetParam(og, "index", OSP_DATA, &id);
  } else {
    indices.resize(m_vertexPosition->totalSize());
    std::iota(indices.begin(), indices.end(), 0);
    auto id = ospNewSharedData(indices.data(), OSP_VEC4UI, indices.size() / 4);
    ospSetParam(og, "index", OSP_DATA, &id);
    ospRelease(id);
  }
  ospCommit(og);

  m_indices = std::move(indices);
}

void Quad::setColorAttribute(Attribute attr, OSPGeometricModel om)
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

void Quad::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");
  ospRemoveParam(og, "texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];
    auto &pa = m_attributes[attrIdx];

    if (va)
      unpackedValues = convertToTexcoordArray(*va);
    else if (pa)
      unpackedValues = convertToTexcoordArray(*pa);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC2F, unpackedValues.size());
      ospSetParam(og, va ? "vertex.texcoord" : "texcoord", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_texcoords = std::move(unpackedValues);
}

} // namespace anari_ospray
