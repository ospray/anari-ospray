// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

// anari
#include <anari/anari_cpp/ext/linalg.h>
#include <anari/anari_cpp.hpp>
// std
#include <cstring>
#include <limits>

namespace anari_ospray {

// Types //////////////////////////////////////////////////////////////////////

using namespace linalg::aliases;
using mat4 = float4x4;
using mat3x4 = float3x4;

constexpr float4 DEFAULT_ATTRIBUTE_VALUE(0.f, 0.f, 0.f, 1.f);

enum class Attribute
{
  ATTRIBUTE_0 = 0,
  ATTRIBUTE_1,
  ATTRIBUTE_2,
  ATTRIBUTE_3,
  COLOR,
  NONE
};

// Functions //////////////////////////////////////////////////////////////////

inline float radians(float degrees)
{
  return degrees * float(M_PI) / 180.f;
}

inline float degrees(float radians)
{
  return radians * 180.f / float(M_PI);
}

inline Attribute attributeFromString(const std::string &str)
{
  if (str == "color")
    return Attribute::COLOR;
  else if (str == "attribute0")
    return Attribute::ATTRIBUTE_0;
  else if (str == "attribute1")
    return Attribute::ATTRIBUTE_1;
  else if (str == "attribute2")
    return Attribute::ATTRIBUTE_2;
  else if (str == "attribute3")
    return Attribute::ATTRIBUTE_3;
  else
    return Attribute::NONE;
}

} // namespace anari_ospray
