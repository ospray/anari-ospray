// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Array.hpp"
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

  // All instances in the scene, if the app set surface/volume and instance,
  // we need to combine the instances with the generated instance for the
  // surface/volume
  IntrusivePtr<Array> m_instancesParam{nullptr};
  OSPData m_allInstances{nullptr};
};

} // namespace ospray
} // namespace anari
