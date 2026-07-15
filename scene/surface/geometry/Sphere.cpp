// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Sphere.h"

namespace anari_ospray {

Sphere::Sphere(OSPRayGlobalState *s)
    : Geometry(s, "sphere"),
      m_index(this),
      m_vertexPosition(this),
      m_vertexRadius(this),
      m_vertexAttributes{this, this, this, this, this}
{}

void Sphere::commitParameters()
{
  Geometry::commitParameters();
  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexRadius = getParamObject<Array1D>("vertex.radius");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");
  m_globalRadius = getParam<float>("radius", 0.01f);
}

void Sphere::finalize()
{
  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on sphere geometry");
    return;
  }

  auto og = osprayGeometry();

  // Remove old parameters //

  ospRemoveParam(og, "sphere.position");
  ospRemoveParam(og, "sphere.radius");

  // Set new parameters //

  ospSetParam(og, "radius", OSP_FLOAT, &m_globalRadius);
  auto vpd = m_vertexPosition->osprayData();
  ospSetParam(og, "sphere.position", OSP_DATA, &vpd);
  if (m_vertexRadius) {
    auto vrd = m_vertexRadius->osprayData();
    ospSetParam(og, "sphere.radius", OSP_DATA, &vrd);
  }
  ospCommit(og);
}

bool Sphere::isValid() const
{
  return m_vertexPosition;
}

void Sphere::setColorAttribute(Attribute attr, OSPGeometricModel om)
{
  ospRemoveParam(om, "color");

  std::vector<float4> unpackedValues;

  if (attr != Attribute::NONE) {
    const auto attrIdx = static_cast<int>(attr);
    const auto &va = m_vertexAttributes[attrIdx];
    const auto &pa = m_attributes[attrIdx];

    if (va)
      unpackedValues = convertToColorArray(*va);
    else if (pa)
      unpackedValues = convertToColorArray(*pa);

    if (!unpackedValues.empty()) {
      const auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC4F, unpackedValues.size());
      ospSetParam(om, "color", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  m_primitiveColors = std::move(unpackedValues);
}

void Sphere::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");
  ospRemoveParam(og, "sphere.texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    const auto attrIdx = static_cast<int>(attr);
    const auto &va = m_vertexAttributes[attrIdx];
    const auto &pa = m_attributes[attrIdx];

    if (va)
      unpackedValues = convertToTexcoordArray(*va);
    else if (pa)
      unpackedValues = convertToTexcoordArray(*pa);

    if (!unpackedValues.empty()) {
      const auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC2F, unpackedValues.size());
      ospSetParam(og, "sphere.texcoord", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  ospCommit(og);
  m_texcoords = std::move(unpackedValues);
}

} // namespace anari_ospray
