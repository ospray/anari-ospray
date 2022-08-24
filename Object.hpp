// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

// ospray
#include "ospray/ospray.h"
// anari
#include "anari/anari_cpp/Traits.h"
#include "anari/backend/utilities/IntrusivePtr.h"
// std
#include <string>

typedef enum : uint32_t
{
  DEFAULT = 0,
  GEOMETRY = 1,
  SPATIAL_FIELD = 1,
  SURFACE = 2,
  VOLUME = 2,
  GROUP = 3,
  INSTANCE = 4,
  WORLD = 5,
  FRAME = 6
} CommitPriority;

#include "util/enumCast.hpp"

namespace anari {
namespace ospray {

struct Object : public RefCounted
{
  Object(OSPObject object = nullptr,
      std::string subtype = "<none>",
      CommitPriority = CommitPriority::DEFAULT);
  virtual ~Object();

  virtual bool getProperty(
      const std::string &name, ANARIDataType type, void *ptr, uint32_t flags);

  virtual void commit();
  virtual void setParam(const char *id, ANARIDataType type, const void *mem);
  virtual void removeParam(const char *id);

  OSPObject handle() const;

  template <typename HANDLE_T>
  HANDLE_T handleAs() const;

  int commitPriority() const;

 protected:
  // Set the object param if it's valid, or remove the param if the object is
  // null
  void setObjectParam(const char *id, ANARIDataType type, const Object *obj);

  OSPObject m_object{nullptr};
  std::string m_subtype{"<none>"};
  CommitPriority m_commitPriority{CommitPriority::DEFAULT};
};

// Inlined definitions ////////////////////////////////////////////////////////

template <typename HANDLE_T>
inline HANDLE_T Object::handleAs() const
{
  return (HANDLE_T)m_object;
}

} // namespace ospray
} // namespace anari
