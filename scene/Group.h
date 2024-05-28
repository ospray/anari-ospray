// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "array/ObjectArray.h"
#include "light/Light.h"
#include "surface/Surface.h"
#include "volume/Volume.h"

namespace anari_ospray {

struct Group : public Object
{
  Group(OSPRayGlobalState *s);
  ~Group() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commit() override;

  void markCommitted() override;

  OSPGroup osprayGroup() const;
  void ospraySceneConstruct();
  void ospraySceneCommit();

 private:
  void cleanup();

  // Geometry //

  helium::ChangeObserverPtr<ObjectArray> m_surfaceData;
  std::vector<OSPGeometricModel> m_geometricModels;

  // Volume //

  helium::ChangeObserverPtr<ObjectArray> m_volumeData;
  std::vector<OSPVolumetricModel> m_volumetricModels;

  // Light //

  helium::ChangeObserverPtr<ObjectArray> m_lightData;
  std::vector<OSPLight> m_lights;

  // BVH //

  struct ObjectUpdates
  {
    helium::TimeStamp lastSceneConstruction{0};
    helium::TimeStamp lastSceneCommit{0};
  } m_objectUpdates;

  OSPGroup m_osprayGroup{nullptr};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Group *, ANARI_GROUP);
