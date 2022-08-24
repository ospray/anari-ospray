// Copyright 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"

namespace anari {
namespace ospray {

struct Array : public Object
{
  Array(const void *appMemory,
      size_t count1,
      size_t count2,
      size_t count3,
      ANARIMemoryDeleter deleter,
      const void *deleterPtr,
      ANARIDataType elementType);

  size_t count1;
  size_t count2;
  size_t count3;
};

} // namespace ospray
} // namespace anari
