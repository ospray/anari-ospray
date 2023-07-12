// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"

namespace anari_ospray {

struct Volume : public Object
{
  Volume(OSPRayGlobalState *d);
  ~Volume();
  static Volume *createInstance(std::string_view subtype, OSPRayGlobalState *d);

  OSPVolumetricModel osprayModel() const;

 private:
  OSPVolumetricModel m_osprayModel{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Volume *, ANARI_VOLUME);
