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
  s->objectCounts.cameras++;
  m_osprayCamera = ospNewCamera(osptype);
  ospCommit(m_osprayCamera);
}

Camera::~Camera()
{
  ospRelease(m_osprayCamera);
  deviceState()->objectCounts.cameras--;
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

void Camera::commit()
{
  auto pos = getParam<float3>("position", float3(0.f));
  auto dir = normalize(getParam<float3>("direction", float3(0.f, 0.f, 1.f)));
  auto up = normalize(getParam<float3>("up", float3(0.f, 1.f, 0.f)));
  float imgRegion[4] = {0.f, 0.f, 1.f, 1.f};
  getParam("imageRegion", ANARI_FLOAT32_BOX2, imgRegion);
  ospSetParam(osprayCamera(), "position", OSP_VEC3F, &pos);
  ospSetParam(osprayCamera(), "direction", OSP_VEC3F, &dir);
  ospSetParam(osprayCamera(), "up", OSP_VEC3F, &up);
  ospSetParam(osprayCamera(), "imageStart", OSP_VEC2F, &imgRegion[0]);
  ospSetParam(osprayCamera(), "imageEnd", OSP_VEC2F, &imgRegion[2]);
  markUpdated();
}

OSPCamera Camera::osprayCamera() const
{
  return m_osprayCamera;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Camera *);
