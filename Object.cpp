// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Object.hpp"
#include <cstring>

namespace anari {
namespace ospray {

Object::Object(
    OSPObject object, std::string subtype, CommitPriority commitPriority)
    : m_object(object), m_subtype(subtype), m_commitPriority(commitPriority)
{}

Object::~Object()
{
  ospRelease(m_object);
}

bool Object::getProperty(
    const std::string &name, ANARIDataType type, void *mem, uint32_t)
{
  if (name == "bounds" && type == ANARI_FLOAT32_BOX3) {
    OSPBounds *b = (OSPBounds *)mem;
    *b = ospGetBounds(handle());
    return true;
  }

  return false;
}

void Object::commit()
{
  if (!m_object)
    return;

  ospCommit(m_object);
}

void Object::setParam(const char *id, ANARIDataType type, const void *mem)
{
  if (!m_object)
    return;

  if (anari::isObject(type)) {
    setObjectParam(id, type, (*(Object **)mem));
  } else {
    // TODO: For transforms we need to change it back to the Mat3x4 used by
    // OSPRay. TODO: Also must apply this to motion transforms, which is more of
    // a pain vs. the single instance transform
    if (m_commitPriority == CommitPriority::INSTANCE
        && std::strcmp(id, "transform") == 0) {
      const float *transform4x4 = reinterpret_cast<const float *>(mem);
      float transform3x4[12] = {// Column 0
          transform4x4[0],
          transform4x4[1],
          transform4x4[2],
          // Column 1
          transform4x4[4],
          transform4x4[5],
          transform4x4[6],
          // Column 2
          transform4x4[8],
          transform4x4[9],
          transform4x4[10],
          // Column 3
          transform4x4[12],
          transform4x4[13],
          transform4x4[14]};
      ospSetParam(m_object, id, OSP_AFFINE3F, transform3x4);
    } else {
      ospSetParam(m_object, id, enumCast<OSPDataType>(type), mem);
    }
  }
}

void Object::removeParam(const char *id)
{
  if (!m_object)
    return;

  ospRemoveParam(m_object, id);
}

void Object::setObjectParam(
    const char *id, ANARIDataType type, const Object *obj)
{
  if (obj) {
    auto handle = obj->handle();
    ospSetParam(m_object, id, enumCast<OSPDataType>(type), &handle);
  } else {
    ospRemoveParam(m_object, id);
  }
}

OSPObject Object::handle() const
{
  return m_object;
}

int Object::commitPriority() const
{
  return m_commitPriority;
}

} // namespace ospray
} // namespace anari
