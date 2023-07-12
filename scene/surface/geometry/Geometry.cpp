// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Geometry.h"
// subtypes
#include "Cone.h"
#include "Curve.h"
#include "Cylinder.h"
#include "Quad.h"
#include "Sphere.h"
#include "Triangle.h"
// std
#include <cstring>
#include <limits>

namespace anari_ospray {

Geometry::Geometry(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_GEOMETRY, s)
{
  s->objectCounts.geometries++;
  m_osprayGeometry = ospNewGeometry(osptype);
}

Geometry::~Geometry()
{
  ospRelease(m_osprayGeometry);
  deviceState()->objectCounts.geometries--;
}

Geometry *Geometry::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "cone")
    return new Cone(s);
  else if (subtype == "curve")
    return new Curve(s);
  else if (subtype == "cylinder")
    return new Cylinder(s);
  else if (subtype == "quad")
    return new Quad(s);
  else if (subtype == "sphere")
    return new Sphere(s);
  else if (subtype == "triangle")
    return new Triangle(s);
  else
    return (Geometry *)new UnknownObject(ANARI_GEOMETRY, s);
}

OSPGeometry Geometry::osprayGeometry() const
{
  return m_osprayGeometry;
}

void Geometry::commit()
{
  m_attributes[0] = getParamObject<Array1D>("primitive.attribute0");
  m_attributes[1] = getParamObject<Array1D>("primitive.attribute1");
  m_attributes[2] = getParamObject<Array1D>("primitive.attribute2");
  m_attributes[3] = getParamObject<Array1D>("primitive.attribute3");
  m_attributes[4] = getParamObject<Array1D>("primitive.color");
}

void Geometry::markCommitted()
{
  Object::markCommitted();
  deviceState()->objectUpdates.lastBLSCommitSceneRequest =
      helium::newTimeStamp();
}

void Geometry::setColorAttribute(Attribute attr, OSPGeometricModel om)
{
  ospRemoveParam(om, "color");

  std::vector<float4> unpackedValues;

  if (attr != Attribute::NONE) {
    auto attrIdx = static_cast<int>(attr);
    auto &pa = m_attributes[attrIdx];

    if (pa)
      unpackedValues = convertToColorArray(*pa);

    if (!unpackedValues.empty()) {
      auto d = ospNewSharedData1D(
          unpackedValues.data(), OSP_VEC4F, unpackedValues.size());
      ospSetParam(om, "color", OSP_DATA, &d);
      ospRelease(d);
    }
  }

  m_primitiveColors = std::move(unpackedValues);
}

void Geometry::setTextureCoordinateAttribute(Attribute attr)
{
  // TODO
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Geometry *);
