// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Instance.h"

namespace anari_ospray {

Instance::Instance(OSPRayGlobalState *s) : Object(ANARI_INSTANCE, s)
{
  m_osprayInstance = ospNewInstance();
}

Instance::~Instance()
{
  ospRelease(m_osprayInstance);
}

void Instance::commitParameters()
{
  m_group = getParamObject<Group>("group");
  m_xfmSet = getParam("transform", ANARI_FLOAT32_MAT4, &m_xfm);
}

void Instance::finalize()
{
  if (!m_group) {
    reportMessage(ANARI_SEVERITY_WARNING, "missing 'group' on ANARIInstance");
    return;
  }

  auto g = m_group->osprayGroup();
  auto i = osprayInstance();

  ospSetParam(i, "group", OSP_GROUP, &g);

  if (m_xfmSet) {
    mat3x4 a3f;
    a3f[0] = float3(m_xfm[0].x, m_xfm[0].y, m_xfm[0].z);
    a3f[1] = float3(m_xfm[1].x, m_xfm[1].y, m_xfm[1].z);
    a3f[2] = float3(m_xfm[2].x, m_xfm[2].y, m_xfm[2].z);
    a3f[3] = float3(m_xfm[3].x, m_xfm[3].y, m_xfm[3].z);
    ospSetParam(i, "transform", OSP_AFFINE3F, &a3f);
  } else
    ospRemoveParam(i, "transform");

  ospCommit(i);
}

void Instance::markFinalized()
{
  Object::markFinalized();
  deviceState()->objectUpdates.lastTLSReconstructSceneRequest =
      helium::newTimeStamp();
}

bool Instance::isValid() const
{
  return m_group;
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

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Instance *);
