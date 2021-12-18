// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.hpp"

namespace anari {
namespace ospray {

struct World : public Object
{
  World();
  ~World() override;
  void commit() override;
  void setParam(const char *id, ANARIDataType type, const void *mem) override;

 private:
  bool needInst{false};
  OSPInstance m_instance{nullptr};
  OSPGroup m_group{nullptr};
};

} // namespace ospray
} // namespace anari
