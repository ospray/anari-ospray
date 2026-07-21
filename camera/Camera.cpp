// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Camera.h"
// specific types
#include "Omnidirectional.h"
#include "Orthographic.h"
#include "Perspective.h"
// std
#include <string>

namespace anari_ospray {

static uint32_t stereoModeFromString(const std::string &mode)
{
  if (mode == "left")
    return OSP_STEREO_LEFT;
  else if (mode == "right")
    return OSP_STEREO_RIGHT;
  else if (mode == "sideBySide")
    return OSP_STEREO_SIDE_BY_SIDE;
  else if (mode == "topBottom")
    return OSP_STEREO_TOP_BOTTOM;
  else
    return OSP_STEREO_NONE;
}

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
  else if (type == "omnidirectional")
    return new Omnidirectional(s);
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
  // 'far' has no OSPRay equivalent
  m_near = getParam<float>("near", 1e-6f);
  m_apertureRadius = getParam<float>("apertureRadius", 0.f);
  m_focusDistance = getParam<float>("focusDistance", 1.f);
  m_stereoMode = stereoModeFromString(getParamString("stereoMode", "none"));
  m_interpupillaryDistance = getParam<float>("interpupillaryDistance", 0.0635f);
  // unset 'aspect' is inherited from the frame dimensions (see setFrameAspect)
  float aspect = 1.f;
  if (getParam("aspect", ANARI_FLOAT32, &aspect))
    m_aspect = aspect;
  else
    m_aspect.reset();
}

void Camera::finalize()
{
  ospSetParam(osprayCamera(), "position", OSP_VEC3F, &m_pos);
  ospSetParam(osprayCamera(), "direction", OSP_VEC3F, &m_dir);
  ospSetParam(osprayCamera(), "up", OSP_VEC3F, &m_up);
  ospSetParam(osprayCamera(), "imageStart", OSP_VEC2F, &m_imgRegion[0]);
  ospSetParam(osprayCamera(), "imageEnd", OSP_VEC2F, &m_imgRegion[2]);
  ospSetParam(osprayCamera(), "nearClip", OSP_FLOAT, &m_near);
  markUpdated();
}

void Camera::setFrameAspect(float) {}

void Camera::applyAspect(float frameAspect)
{
  const float aspect = m_aspect.value_or(frameAspect);
  if (aspect == m_committedAspect)
    return;
  m_committedAspect = aspect;
  ospSetParam(osprayCamera(), "aspect", OSP_FLOAT, &aspect);
  ospCommit(osprayCamera());
}

OSPCamera Camera::osprayCamera() const
{
  return m_osprayCamera;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Camera *);
