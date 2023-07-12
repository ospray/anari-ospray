// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Instance.h"

namespace anari_ospray {

Instance::Instance(OSPRayGlobalState *s) : Object(ANARI_INSTANCE, s)
{
  s->objectCounts.instances++;
  m_osprayInstance = ospNewInstance();
}

Instance::~Instance()
{
  ospRelease(m_osprayInstance);
  deviceState()->objectCounts.instances--;
}

void Instance::commit()
{
  m_group = getParamObject<Group>("group");
  if (!m_group) {
    reportMessage(ANARI_SEVERITY_WARNING, "missing 'group' on ANARIInstance");
    return;
  }

  auto g = m_group->osprayGroup();
  auto i = osprayInstance();

  ospSetParam(i, "group", OSP_GROUP, &g);

  mat4 xfm = linalg::identity;
  if (getParam("transform", ANARI_FLOAT32_MAT4, &xfm)) {
    mat3x4 a3f;
    a3f[0] = float3(xfm[0].x, xfm[0].y, xfm[0].z);
    a3f[1] = float3(xfm[1].x, xfm[1].y, xfm[1].z);
    a3f[2] = float3(xfm[2].x, xfm[2].y, xfm[2].z);
    a3f[3] = float3(xfm[3].x, xfm[3].y, xfm[3].z);
    ospSetParam(i, "transform", OSP_AFFINE3F, &a3f);
  } else
    ospRemoveParam(i, "transform");

  ospCommit(i);
}

const Group *Instance::group() const
{
  return m_group.ptr;
}

Group *Instance::group()
{
  return m_group.ptr;
}

OSPInstance Instance::osprayInstance() const
{
  return m_osprayInstance;
}

void Instance::markCommitted()
{
  Object::markCommitted();
  deviceState()->objectUpdates.lastTLSReconstructSceneRequest =
      helium::newTimeStamp();
}

bool Instance::isValid() const
{
  return m_group;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Instance *);
