// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"
#include "util/ParamWrapper.hpp"
// std
#include <vector>

namespace anari {
namespace ospray {

struct Geometry : public Object
{
  Geometry(std::string subtype);
  void setParam(const char *id, ANARIDataType type, const void *mem) override;
  std::vector<ParamWrapper> m_modelParams;
};

} // namespace ospray
} // namespace anari
