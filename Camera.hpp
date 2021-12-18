// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"

namespace anari {
namespace ospray {

struct Camera : public Object
{
  Camera(std::string subtype);
};

} // namespace ospray
} // namespace anari
