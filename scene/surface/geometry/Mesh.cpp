// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Mesh.h"
// std
#include <numeric>

namespace anari_ospray {

Mesh::Mesh(OSPRayGlobalState *s, Subtype subtype)
    : Geometry(s, "mesh"),
      m_subtype(subtype),
      m_index(this),
      m_vertexPosition(this),
      m_vertexNormal(this),
      m_vertexAttributes{this, this, this, this, this},
      m_faceVaryingNormal(this),
      m_faceVaryingAttributes{this, this, this, this, this}
{}

void Mesh::commitParameters()
{
  Geometry::commitParameters();
  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexNormal = getParamObject<Array1D>("vertex.normal");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
  m_faceVaryingNormal = getParamObject<Array1D>("faceVarying.normal");
  m_faceVaryingAttributes[0] =
      getParamObject<Array1D>("faceVarying.attribute0");
  m_faceVaryingAttributes[1] =
      getParamObject<Array1D>("faceVarying.attribute1");
  m_faceVaryingAttributes[2] =
      getParamObject<Array1D>("faceVarying.attribute2");
  m_faceVaryingAttributes[3] =
      getParamObject<Array1D>("faceVarying.attribute3");
  m_faceVaryingAttributes[4] =
      getParamObject<Array1D>("faceVarying.color");
}

void Mesh::finalize()
{
  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on triangle/quad geometry");
    return;
  }

  auto og = osprayGeometry();

  std::vector<uint32_t> indices;

  // Remove old parameters //

  ospRemoveParam(og, "vertex.position");
  ospRemoveParam(og, "vertex.normal");
  ospRemoveParam(og, "normal");
  ospRemoveParam(og, "index");

  // Set new parameters //

  auto vpd = m_vertexPosition->osprayData();
  ospSetParam(og, "vertex.position", OSP_DATA, &vpd);
  if (m_faceVaryingNormal) {
    auto fnd = m_faceVaryingNormal->osprayData();
    ospSetParam(og, "normal", OSP_DATA, &fnd);
  } else if (m_vertexNormal) {
    auto vnd = m_vertexNormal->osprayData();
    ospSetParam(og, "vertex.normal", OSP_DATA, &vnd);
  }

  if (m_index) {
    auto id = m_index->osprayData();
    ospSetParam(og, "index", OSP_DATA, &id);
  } else {
    indices.resize(m_vertexPosition->size());
    std::iota(indices.begin(), indices.end(), 0);
    auto id = ospNewSharedData(indices.data(),
        m_subtype == Subtype::TRIANGLE ? OSP_VEC3UI : OSP_VEC4UI,
        indices.size() / (m_subtype == Subtype::TRIANGLE ? 3 : 4));
    ospSetParam(og, "index", OSP_DATA, &id);
    ospRelease(id);
  }
  ospCommit(og);

  m_indices = std::move(indices);
}

bool Mesh::isValid() const
{
  return m_vertexPosition;
}

void Mesh::setColorAttribute(Attribute attr, OSPGeometricModel om)
{
  Geometry::setColorAttribute(attr, om);

  std::vector<float4> unpackedValues;

  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.color");
  ospRemoveParam(og, "color");

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];
    auto &fa = m_faceVaryingAttributes[attrIdx];

    if (fa)
      unpackedValues = convertToColorArray(*fa);
    else if (va)
      unpackedValues = convertToColorArray(*va);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC4F, unpackedValues.size());
      ospSetParam(og, fa ? "color" : "vertex.color", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_colors = std::move(unpackedValues);
}

void Mesh::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");
  ospRemoveParam(og, "texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_vertexAttributes[attrIdx];
    auto &fa = m_faceVaryingAttributes[attrIdx];
    auto &pa = m_attributes[attrIdx];

    if (fa)
      unpackedValues = convertToTexcoordArray(*fa);
    else if (va)
      unpackedValues = convertToTexcoordArray(*va);
    else if (pa)
      unpackedValues = convertToTexcoordArray(*pa);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC2F, unpackedValues.size());
      ospSetParam(
          og, va && !fa ? "vertex.texcoord" : "texcoord", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_texcoords = std::move(unpackedValues);
}

} // namespace anari_ospray
