// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "../Object.h"
// std
#include <optional>

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

  // push the frame-derived aspect ratio; no-op for cameras without 'aspect'
  virtual void setFrameAspect(float frameAspect);

 protected:
  // set OSPRay 'aspect', preferring the explicit value over the frame's
  void applyAspect(float frameAspect);

  OSPCamera m_osprayCamera{nullptr};
  float3 m_pos{0.f, 0.f, 0.f};
  float3 m_dir{0.f, 0.f, 1.f};
  float3 m_up{0.f, 1.f, 0.f};
  float m_imgRegion[4];
  float m_near{1e-6f};
  float m_apertureRadius{0.f};
  float m_focusDistance{1.f};
  uint32_t m_stereoMode{OSP_STEREO_NONE};
  float m_interpupillaryDistance{0.0635f};
  std::optional<float> m_aspect; // unset: derive aspect from frame dimensions
  float m_committedAspect{-1.f};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Camera *, ANARI_CAMERA);
