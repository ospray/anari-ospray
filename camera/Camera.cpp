// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Camera.h"
// specific types
#include "Orthographic.h"
#include "Perspective.h"

namespace anari_ospray {

Camera::Camera(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_CAMERA, s)
{
  m_osprayCamera = ospNewCamera(osptype);
  ospCommit(m_osprayCamera);
}

Camera::~Camera()
{
  ospRelease(m_osprayCamera);
}

Camera *Camera::createInstance(std::string_view type, OSPRayGlobalState *s)
{
  if (type == "perspective")
    return new Perspective(s);
  else if (type == "orthographic")
    return new Orthographic(s);
  else
    return (Camera *)new UnknownObject(ANARI_CAMERA, s);
}

void Camera::commitParameters()
{
  m_pos = getParam<float3>("position", float3(0.f));
  m_dir = normalize(getParam<float3>("direction", float3(0.f, 0.f, 1.f)));
  m_up = normalize(getParam<float3>("up", float3(0.f, 1.f, 0.f)));
  if (!getParam("imageRegion", ANARI_FLOAT32_BOX2, m_imgRegion)) {
    m_imgRegion[0] = 0.f;
    m_imgRegion[1] = 0.f;
    m_imgRegion[2] = 1.f;
    m_imgRegion[3] = 1.f;
  }
}

void Camera::finalize()
{
  ospSetParam(osprayCamera(), "position", OSP_VEC3F, &m_pos);
  ospSetParam(osprayCamera(), "direction", OSP_VEC3F, &m_dir);
  ospSetParam(osprayCamera(), "up", OSP_VEC3F, &m_up);
  ospSetParam(osprayCamera(), "imageStart", OSP_VEC2F, &m_imgRegion[0]);
  ospSetParam(osprayCamera(), "imageEnd", OSP_VEC2F, &m_imgRegion[2]);
  markUpdated();
}

OSPCamera Camera::osprayCamera() const
{
  return m_osprayCamera;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Camera *);
