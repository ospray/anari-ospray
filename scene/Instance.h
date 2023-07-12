// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Group.h"

namespace anari_ospray {

struct Instance : public Object
{
  Instance(OSPRayGlobalState *s);
  ~Instance() override;

  void commit() override;

  const Group *group() const;
  Group *group();

  OSPInstance osprayInstance() const;

  void markCommitted() override;

  bool isValid() const override;

 private:
  helium::IntrusivePtr<Group> m_group;
  OSPInstance m_osprayInstance{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Instance *, ANARI_INSTANCE);
