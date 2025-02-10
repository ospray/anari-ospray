// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"

namespace anari_ospray {

struct Camera : public Object
{
  Camera(OSPRayGlobalState *s, const char *osptype);
  ~Camera() override;

  void commitParameters() override;
  void finalize() override;

  static Camera *createInstance(
      std::string_view type, OSPRayGlobalState *state);

  OSPCamera osprayCamera() const;

 protected:
  OSPCamera m_osprayCamera{nullptr};
  float3 m_pos{0.f, 0.f, 0.f};
  float3 m_dir{0.f, 0.f, 1.f};
  float3 m_up{0.f, 1.f, 0.f};
  float m_imgRegion[4];
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Camera *, ANARI_CAMERA);
