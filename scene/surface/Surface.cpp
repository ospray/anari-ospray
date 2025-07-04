// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Surface.h"

namespace anari_ospray {

Surface::Surface(OSPRayGlobalState *s)
    : Object(ANARI_SURFACE, s), m_geometry(this)
{
  m_osprayModel = ospNewGeometricModel();
}

Surface::~Surface()
{
  ospRelease(m_osprayModel);
}

void Surface::commitParameters()
{
  m_geometry = getParamObject<Geometry>("geometry");
  m_material = getParamObject<Material>("material");
}

void Surface::finalize()
{
  if (!m_material || !m_material->isValid()) {
    reportMessage(ANARI_SEVERITY_WARNING, "missing 'material' on ANARISurface");
    return;
  }

  if (!m_geometry || !m_geometry->isValid()) {
    reportMessage(ANARI_SEVERITY_WARNING, "missing 'geometry' on ANARISurface");
    return;
  }

  auto om = osprayModel();

  m_geometry->setColorAttribute(m_material->colorAttribute(), om);
  m_geometry->setTextureCoordinateAttribute(m_material->texcoordAttribute());

  auto g = m_geometry->osprayGeometry();
  auto m = m_material->osprayMaterial();
  ospSetParam(om, "geometry", OSP_GEOMETRY, &g);
  ospSetParam(om, "material", OSP_MATERIAL, &m);
  ospCommit(om);
}

void Surface::markFinalized()
{
  Object::markFinalized();
  deviceState()->objectUpdates.lastBLSReconstructSceneRequest =
      helium::newTimeStamp();
}

bool Surface::isValid() const
{
  bool allowInvalidMaterial = deviceState()->allowInvalidSurfaceMaterials;

  if (allowInvalidMaterial) {
    return m_geometry && m_geometry->isValid();
  } else {
    return m_geometry && m_material && m_geometry->isValid()
        && m_material->isValid();
  }
}

const Geometry *Surface::geometry() const
{
  return m_geometry.get();
}

const Material *Surface::material() const
{
  return m_material.ptr;
}

OSPGeometricModel Surface::osprayModel() const
{
  return m_osprayModel;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Surface *);
