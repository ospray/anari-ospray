// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Geometry.hpp"
// std
#include <vector>

namespace anari {
namespace ospray {

struct Surface : public Object
{
  Surface();

  void commit() override;
  void setParam(const char *id, ANARIDataType type, const void *mem) override;

 private:
  IntrusivePtr<Geometry> m_geometry;
};

} // namespace ospray
} // namespace anari
