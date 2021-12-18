// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <cstddef>
#include <string>

namespace anari {
namespace ospray {

struct ParamWrapper
{
  ParamWrapper(std::string id, int type, const void *mem);
  ~ParamWrapper();

  ParamWrapper(const ParamWrapper &copy);

  const char *id() const;
  int type() const;
  const void *mem() const;

 private:
  std::string m_id;
  int m_type{-1};
  std::array<unsigned char, 4 * 3 * sizeof(float)> m_mem;
};

} // namespace ospray
} // namespace anari
