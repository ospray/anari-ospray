// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "World.hpp"
#include "ospray/ospray_util.h"

namespace anari {
namespace ospray {

World::World() : Object(ospNewWorld(), "<none>", CommitPriority::WORLD)
{
  m_group = ospNewGroup();
}

World::~World()
{
  ospRelease(m_instance);
  ospRelease(m_group);
  ospRelease(m_allInstances);
}

void World::commit()
{
  if (needInst) {
    ospCommit(m_group);
    if (!m_instance)
      m_instance = ospNewInstance(m_group);
    ospCommit(m_instance);
  }
  // If we have a list of instances we need to add the generated instance to
  // the list so that they're all included in the world
  if (m_instance && m_instancesParam) {
    ospRelease(m_allInstances);
    m_allInstances =
        ospNewData(OSP_INSTANCE, m_instancesParam->count1 + 1, 1, 1);
    ospCopyData((OSPData)m_instancesParam->handle(), m_allInstances);

    OSPData tmp = ospNewSharedData(&m_instance, OSP_INSTANCE, 1);
    ospCopyData(tmp, m_allInstances, m_instancesParam->count1);
    ospSetParam(m_object, "instance", OSP_DATA, &m_allInstances);
    ospRelease(tmp);
  } else if (m_instancesParam) {
    auto handle = m_instancesParam->handle();
    ospSetParam(m_object, "instance", OSP_DATA, &handle);
  } else if (m_instance) {
    ospSetObjectAsData(m_object, "instance", OSP_INSTANCE, m_instance);
  }

  Object::commit();
}

void World::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);
  if (id == "surface") {
    needInst = true;
    auto *obj = (*(Object **)mem);
    if (obj) {
      auto handle = obj->handle();
      ospSetParam(m_group, "geometry", enumCast<OSPDataType>(type), &handle);
    } else {
      ospRemoveParam(m_group, "geometry");
    }
  } else if (id == "volume") {
    needInst = true;
    auto *obj = (*(Object **)mem);
    if (obj) {
      auto handle = obj->handle();
      ospSetParam(m_group, "volume", enumCast<OSPDataType>(type), &handle);
    } else {
      ospRemoveParam(m_group, "volume");
    }
  } else if (id == "instance") {
    // We need to catch instances so that we can combine them with the
    // generated instance for any surface or volume parameters
    m_instancesParam = (*(Array **)mem);
  } else {
    Object::setParam(id.c_str(), type, mem);
  }
}

} // namespace ospray
} // namespace anari
