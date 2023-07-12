// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"

namespace anari_ospray {

struct Camera : public Object
{
  Camera(OSPRayGlobalState *s, const char *osptype);
  ~Camera() override;

  virtual void commit() override;

  static Camera *createInstance(
      std::string_view type, OSPRayGlobalState *state);

  OSPCamera osprayCamera() const;

 protected:
  OSPCamera m_osprayCamera{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Camera *, ANARI_CAMERA);
