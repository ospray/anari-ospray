// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Object.h"
// std
#include <atomic>
#include <cstdarg>

namespace anari_ospray {

// Object definitions /////////////////////////////////////////////////////////

Object::Object(ANARIDataType type, OSPRayGlobalState *s)
    : helium::BaseObject(type, s)
{
  helium::BaseObject::markParameterChanged();
  s->commitBuffer.addObjectToCommit(this);
}

void Object::commitParameters()
{
  // no-op
}

void Object::finalize()
{
  // no-op
}

bool Object::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (name == "valid" && type == ANARI_BOOL) {
    helium::writeToVoidP(ptr, isValid());
    return true;
  }

  return false;
}

bool Object::isValid() const
{
  return true;
}

OSPRayGlobalState *Object::deviceState() const
{
  return (OSPRayGlobalState *)helium::BaseObject::m_state;
}

// UnknownObject definitions //////////////////////////////////////////////////

UnknownObject::UnknownObject(ANARIDataType type, OSPRayGlobalState *s)
    : Object(type, s)
{
  reportMessage(
      ANARI_SEVERITY_WARNING, "created unknown %s", anari::toString(type));
}

bool UnknownObject::isValid() const
{
  return false;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Object *);
