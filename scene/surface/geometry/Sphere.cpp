// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Sphere.h"

namespace anari_ospray {

Sphere::Sphere(OSPRayGlobalState *s) : Geometry(s, "sphere"), m_index(this), m_vertexPosition(this), m_vertexRadius(this) {}

void Sphere::commit()
{
  Geometry::commit();

  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexRadius = getParamObject<Array1D>("vertex.radius");
  m_attributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_attributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_attributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_attributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_attributes[4] = getParamObject<Array1D>("vertex.color");

  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on sphere geometry");
    return;
  }

  m_globalRadius = getParam<float>("radius", 0.01f);

  const float *radius = nullptr;
  if (m_vertexRadius)
    radius = m_vertexRadius->beginAs<float>();

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

void Sphere::setTextureCoordinateAttribute(Attribute attr)
{
  auto og = osprayGeometry();
  ospRemoveParam(og, "vertex.texcoord");

  std::vector<float2> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &va = m_attributes[attrIdx];

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
