// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"
// std
#include <vector>

namespace anari {
namespace ospray {

struct Volume : public Object
{
  Volume();

  void commit() override;
  void setParam(const char *id, ANARIDataType type, const void *mem) override;

 private:
  Object m_tf;
};

} // namespace ospray
} // namespace anari
