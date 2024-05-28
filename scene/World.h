// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.h"

namespace anari_ospray {

struct World : public Object
{
  World(OSPRayGlobalState *s);
  ~World() override;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;

  void commit() override;

  const std::vector<Instance *> &instances() const;

  OSPWorld osprayWorld() const;
  void osprayWorldUpdate();

  void setAmbientLightValues(float3 color, float intensity);

 private:
  void rebuildBLSs();
  void recommitBLSs();
  void rebuildTLS();

  helium::ChangeObserverPtr<ObjectArray> m_zeroSurfaceData;
  helium::ChangeObserverPtr<ObjectArray> m_zeroVolumeData;
  helium::ChangeObserverPtr<ObjectArray> m_zeroLightData;

  helium::ChangeObserverPtr<ObjectArray> m_instanceData;
  std::vector<Instance *> m_instances;
  std::vector<OSPInstance> m_osprayInstances;

  helium::IntrusivePtr<Group> m_zeroGroup;
  helium::IntrusivePtr<Instance> m_zeroInstance;

  struct ObjectUpdates
  {
    helium::TimeStamp lastTLSBuild{0};
    helium::TimeStamp lastBLSReconstructCheck{0};
    helium::TimeStamp lastBLSCommitCheck{0};
  } m_objectUpdates;

  OSPWorld m_osprayWorld{nullptr};
  OSPLight m_osprayAmbientLight{nullptr};

  float3 m_ambientColor{1.f, 1.f, 1.f};
  float m_ambientRadiance{0.f};
};

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::World *, ANARI_WORLD);
