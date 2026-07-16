// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
// std
#include <string>
#include <vector>

namespace anari_ospray {

struct Geometry;

struct Sampler : public Object
{
  Sampler(OSPRayGlobalState *d);
  ~Sampler();

  static Sampler *createInstance(
      std::string_view subtype, OSPRayGlobalState *d);

  void commitParameters() override;

  OSPTexture osprayTexture() const;
  virtual Attribute inAttribute() const = 0;

  // 'inTransform'/'inOffset' -> material map slot (e.g. "map_baseColor")
  void applyInTransform(OSPMaterial om, const char *mapName) const;

 protected:
  static uint32_t wrapModeFromString(const std::string &str);

  // bake 'outTransform'/'outOffset' into texels (affine commutes with filtering)
  void applyOutTransform(std::vector<float4> &colors) const;

  // must the out-transform be baked (blocking the native path)? a 'copy R to
  // G,B' transform is redundant for L8/LA8 (which already broadcast) -> false
  bool outTransformNeedsBake(OSPTextureFormat format) const;

  OSPTexture m_osprayTexture{nullptr};

  mat4 m_inTransform{linalg::identity};
  float4 m_inOffset{0.f};
  mat4 m_outTransform{linalg::identity};
  float4 m_outOffset{0.f};
  bool m_hasInTransform{false};
  bool m_hasOutTransform{false};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Sampler *, ANARI_SAMPLER);
