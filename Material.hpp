// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"

namespace anari {
namespace ospray {

struct Material : public Object
{
  Material(std::string subtype);
  void setParam(const char *_id, ANARIDataType type, const void *mem) override;
};

} // namespace ospray
} // namespace anari
