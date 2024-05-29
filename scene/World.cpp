// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "World.h"

namespace anari_ospray {

World::World(OSPRayGlobalState *s) : Object(ANARI_WORLD, s), m_zeroSurfaceData(this), m_zeroVolumeData(this), m_zeroLightData(this), m_instanceData(this)
{
  s->objectCounts.worlds++;

  m_osprayAmbientLight = ospNewLight("ambient");
  ospSetBool(m_osprayAmbientLight, "visible", false);
  setAmbientLightValues(float3(1, 1, 1), 0.25f);

  m_osprayWorld = ospNewWorld();
  ospSetObjectAsData(m_osprayWorld, "light", OSP_LIGHT, m_osprayAmbientLight);
  ospCommit(m_osprayWorld);

  m_zeroGroup = new Group(s);
  m_zeroInstance = new Instance(s);
  m_zeroInstance->setParamDirect("group", m_zeroGroup.ptr);

  // never any public ref to these objects
  m_zeroGroup->refDec(helium::RefType::PUBLIC);
  m_zeroInstance->refDec(helium::RefType::PUBLIC);
}

World::~World()
{
  ospRelease(m_osprayWorld);
  ospRelease(m_osprayAmbientLight);
  deviceState()->objectCounts.worlds--;
}

bool World::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (name == "bounds" && type == ANARI_FLOAT32_BOX3) {
    if (flags & ANARI_WAIT) {
      deviceState()->waitOnCurrentFrame();
      deviceState()->commitBufferFlush();
      osprayWorldUpdate();
    }
    auto bounds = ospGetBounds(m_osprayWorld);
    std::memcpy(ptr, &bounds, sizeof(bounds));
    return true;
  }

  return Object::getProperty(name, type, ptr, flags);
}

void World::commit()
{
  m_zeroSurfaceData = getParamObject<ObjectArray>("surface");
  m_zeroVolumeData = getParamObject<ObjectArray>("volume");
  m_zeroLightData = getParamObject<ObjectArray>("light");

  const bool addZeroInstance = m_zeroSurfaceData || m_zeroVolumeData || m_zeroLightData;
  if (addZeroInstance)
    reportMessage(
        ANARI_SEVERITY_DEBUG, "anari_ospray::World will add zero instance");

  if (m_zeroSurfaceData) {
    reportMessage(ANARI_SEVERITY_DEBUG,
        "anari_ospray::World found %zu surfaces in zero instance",
        m_zeroSurfaceData->size());
    m_zeroGroup->setParamDirect("surface", getParamDirect("surface"));
  } else
    m_zeroGroup->removeParam("surface");

  if (m_zeroVolumeData) {
    reportMessage(ANARI_SEVERITY_DEBUG,
        "anari_ospray::World found %zu volumes in zero instance",
        m_zeroVolumeData->size());
    m_zeroGroup->setParamDirect("volume", getParamDirect("volume"));
  } else
    m_zeroGroup->removeParam("volume");

  if (m_zeroLightData) {
    reportMessage(ANARI_SEVERITY_DEBUG,
        "anari_ospray::World found %zu lights in zero instance",
        m_zeroLightData->size());
    m_zeroGroup->setParamDirect("light", getParamDirect("light"));
  } else
    m_zeroGroup->removeParam("light");

  m_zeroGroup->commit();
  m_zeroInstance->commit();

  m_instanceData = getParamObject<ObjectArray>("instance");

  m_instances.clear();

  if (m_instanceData) {
    std::for_each(m_instanceData->handlesBegin(),
        m_instanceData->handlesEnd(),
        [&](Object *o) {
          if (o && o->isValid())
            m_instances.push_back((Instance *)o);
        });
  }

  if (addZeroInstance)
    m_instances.push_back(m_zeroInstance.ptr);

  m_objectUpdates.lastTLSBuild = 0;
  m_objectUpdates.lastBLSReconstructCheck = 0;
  m_objectUpdates.lastBLSCommitCheck = 0;

  if (m_instanceData)
    m_instanceData->addChangeObserver(this);
  if (m_zeroSurfaceData)
    m_zeroSurfaceData->addChangeObserver(this);
}

const std::vector<Instance *> &World::instances() const
{
  return m_instances;
}

OSPWorld World::osprayWorld() const
{
  return m_osprayWorld;
}

void World::osprayWorldUpdate()
{
  rebuildBLSs();
  recommitBLSs();
  rebuildTLS();
}

void World::setAmbientLightValues(float3 color, float intensity)
{
  auto &state = *deviceState();

  auto oal = m_osprayAmbientLight;
  ospSetParam(oal, "color", OSP_VEC3F, &color);
  ospSetParam(oal, "intensity", OSP_FLOAT, &intensity);

  if (color != m_ambientColor || intensity != m_ambientRadiance)
    state.objectUpdates.lastTLSReconstructSceneRequest = helium::newTimeStamp();

  m_ambientColor = color;
  m_ambientRadiance = intensity;

  ospCommit(oal);
}

void World::rebuildBLSs()
{
  const auto &state = *deviceState();
  if (state.objectUpdates.lastBLSReconstructSceneRequest
      < m_objectUpdates.lastBLSReconstructCheck) {
    return;
  }

  m_objectUpdates.lastTLSBuild = 0; // BLS changed, so need to build TLS
  reportMessage(ANARI_SEVERITY_DEBUG,
      "anari_ospray::World rebuilding %zu BLSs",
      m_instances.size());
  std::for_each(m_instances.begin(), m_instances.end(), [&](auto *inst) {
    inst->group()->ospraySceneConstruct();
  });

  m_objectUpdates.lastBLSReconstructCheck = helium::newTimeStamp();
  m_objectUpdates.lastBLSCommitCheck = helium::newTimeStamp();
}

void World::recommitBLSs()
{
  const auto &state = *deviceState();
  if (state.objectUpdates.lastBLSCommitSceneRequest
      < m_objectUpdates.lastBLSCommitCheck) {
    return;
  }

  m_objectUpdates.lastTLSBuild = 0; // BLS changed, so need to build TLS
  reportMessage(ANARI_SEVERITY_DEBUG,
      "anari_ospray::World recommitting %zu BLSs",
      m_instances.size());
  std::for_each(m_instances.begin(), m_instances.end(), [&](auto *inst) {
    inst->group()->ospraySceneCommit();
  });

  m_objectUpdates.lastBLSCommitCheck = helium::newTimeStamp();
}

void World::rebuildTLS()
{
  const auto &state = *deviceState();
  if (state.objectUpdates.lastTLSReconstructSceneRequest
      < m_objectUpdates.lastTLSBuild) {
    return;
  }

  reportMessage(ANARI_SEVERITY_DEBUG,
      "anari_ospray::World rebuilding TLS over %zu instances",
      m_instances.size());

  std::vector<OSPInstance> osprayInstances;

  std::for_each(m_instances.begin(), m_instances.end(), [&](auto *i) {
    if (i && i->isValid())
      osprayInstances.push_back(i->osprayInstance());
    else {
      reportMessage(ANARI_SEVERITY_DEBUG,
          "anari_ospray::World rejecting invalid surfaces in instance(%p) "
          "when building TLS",
          i);
    }
  });

  auto w = osprayWorld();

  if (!osprayInstances.empty()) {
    auto d = ospNewSharedData1D(
        osprayInstances.data(), OSP_INSTANCE, osprayInstances.size());
    ospSetParam(w, "instance", OSP_DATA, &d);
    ospRelease(d);
  } else
    ospRemoveParam(w, "instance");

  ospCommit(w);
  m_objectUpdates.lastTLSBuild = helium::newTimeStamp();

  m_osprayInstances = std::move(osprayInstances);
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::World *);
