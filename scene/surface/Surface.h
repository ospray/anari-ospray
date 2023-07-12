// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "scene/surface/geometry/Geometry.h"
#include "scene/surface/material/Material.h"

namespace anari_ospray {

struct Surface : public Object
{
  Surface(OSPRayGlobalState *s);
  ~Surface() override;

  void commit() override;

  const Geometry *geometry() const;
  const Material *material() const;

  void markCommitted() override;
  bool isValid() const override;

  OSPGeometricModel osprayModel() const;

 private:
  helium::IntrusivePtr<Geometry> m_geometry;
  helium::IntrusivePtr<Material> m_material;

  OSPGeometricModel m_osprayModel{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Surface *, ANARI_SURFACE);
