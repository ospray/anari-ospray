// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "array/Array1D.h"
// std
#include <array>

namespace anari_ospray {

struct Geometry : public Object
{
  Geometry(OSPRayGlobalState *s, const char *osptype);
  ~Geometry() override;

  static Geometry *createInstance(
      std::string_view subtype, OSPRayGlobalState *s);

  OSPGeometry osprayGeometry() const;

  void commit() override;
  void markCommitted() override;

  virtual void setColorAttribute(Attribute attr, OSPGeometricModel om);
  virtual void setTextureCoordinateAttribute(Attribute attr);

 protected:
  OSPGeometry m_osprayGeometry{nullptr};
  std::array<helium::IntrusivePtr<Array1D>, 5> m_attributes;
  std::vector<float4> m_primitiveColors;
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Geometry *, ANARI_GEOMETRY);
