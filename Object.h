// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OSPRayGlobalState.h"
#include "ospray_math.h"
// helium
#include "helium/BaseObject.h"
#include "helium/utility/ChangeObserverPtr.h"
// std
#include <string_view>

namespace anari_ospray {

struct Object : public helium::BaseObject
{
  Object(ANARIDataType type, OSPRayGlobalState *s);
  virtual ~Object() = default;

  virtual bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags);

  virtual void commit();

  virtual bool isValid() const;

  OSPRayGlobalState *deviceState() const;
};

struct UnknownObject : public Object
{
  UnknownObject(ANARIDataType type, OSPRayGlobalState *s);
  ~UnknownObject() override;
  bool isValid() const override;
};

} // namespace anari_ospray

#define OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(type, anari_type)                  \
  namespace anari {                                                            \
  ANARI_TYPEFOR_SPECIALIZATION(type, anari_type);                              \
  }

#define OSPRAY_ANARI_TYPEFOR_DEFINITION(type)                                  \
  namespace anari {                                                            \
  ANARI_TYPEFOR_DEFINITION(type);                                              \
  }

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Object *, ANARI_OBJECT);
