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
}

void World::commit()
{
  if (needInst) {
    ospCommit(m_group);
    if (!m_instance)
      m_instance = ospNewInstance(m_group);
    ospCommit(m_instance);
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
  } else
    Object::setParam(id.c_str(), type, mem);
}

} // namespace ospray
} // namespace anari
