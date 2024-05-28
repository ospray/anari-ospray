// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Group.h"
// std
#include <iterator>

namespace anari_ospray {

Group::Group(OSPRayGlobalState *s) : Object(ANARI_GROUP, s), m_surfaceData(this), m_volumeData(this), m_lightData(this)
{
  s->objectCounts.groups++;
  m_osprayGroup = ospNewGroup();
}

Group::~Group()
{
  cleanup();
  ospRelease(m_osprayGroup);
  m_osprayGroup = nullptr;
  deviceState()->objectCounts.groups--;
}

bool Group::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  if (name == "bounds" && type == ANARI_FLOAT32_BOX3) {
    if (flags & ANARI_WAIT) {
      deviceState()->waitOnCurrentFrame();
      deviceState()->commitBufferFlush();
      ospraySceneConstruct();
      ospraySceneCommit();
    }
    auto bounds = ospGetBounds(m_osprayGroup);
    std::memcpy(ptr, &bounds, sizeof(bounds));
    return true;
  }

  return Object::getProperty(name, type, ptr, flags);
}

void Group::commit()
{
  cleanup();

  m_surfaceData = getParamObject<ObjectArray>("surface");
  m_volumeData = getParamObject<ObjectArray>("volume");
  m_lightData = getParamObject<ObjectArray>("light");
}

void Group::markCommitted()
{
  Object::markCommitted();
  deviceState()->objectUpdates.lastBLSReconstructSceneRequest =
      helium::newTimeStamp();
}

OSPGroup Group::osprayGroup() const
{
  return m_osprayGroup;
}

void Group::ospraySceneConstruct()
{
  const auto &state = *deviceState();
  if (m_objectUpdates.lastSceneConstruction
      > state.objectUpdates.lastBLSReconstructSceneRequest)
    return;

  reportMessage(
      ANARI_SEVERITY_DEBUG, "anari_ospray::Group rebuilding ospray scene");

  std::vector<OSPGeometricModel> geometricModels;
  std::vector<OSPVolumetricModel> volumetricModels;
  std::vector<OSPLight> lights;

  if (m_surfaceData) {
    std::for_each(m_surfaceData->handlesBegin(),
        m_surfaceData->handlesEnd(),
        [&](Object *o) {
          auto *s = (Surface *)o;
          if (s && s->isValid())
            geometricModels.push_back(s->osprayModel());
          else {
            reportMessage(ANARI_SEVERITY_DEBUG,
                "anari_ospray::Group rejecting invalid surface(%p) in building BLS",
                s);
          }
        });
  }

  if (m_volumeData) {
    std::for_each(m_volumeData->handlesBegin(),
        m_volumeData->handlesEnd(),
        [&](Object *o) {
          auto *v = (Volume *)o;
          if (v && v->isValid())
            volumetricModels.push_back(v->osprayModel());
          else {
            reportMessage(ANARI_SEVERITY_DEBUG,
                "anari_ospray::Group rejecting invalid volume(%p) in building BLS",
                v);
          }
        });
  }

  if (m_lightData) {
    std::for_each(
        m_lightData->handlesBegin(), m_lightData->handlesEnd(), [&](Object *o) {
          auto *l = (Light *)o;
          if (l && l->isValid())
            lights.push_back(l->osprayLight());
          else {
            reportMessage(ANARI_SEVERITY_DEBUG,
                "anari_ospray::Group rejecting invalid light(%p) in building BLS",
                l);
          }
        });
  }

  auto g = osprayGroup();

  if (!geometricModels.empty()) {
    auto d = ospNewSharedData1D(
        geometricModels.data(), OSP_GEOMETRIC_MODEL, geometricModels.size());
    ospSetParam(g, "geometry", OSP_DATA, &d);
    ospRelease(d);
  } else
    ospRemoveParam(g, "geometry");

  if (!volumetricModels.empty()) {
    auto d = ospNewSharedData1D(
        volumetricModels.data(), OSP_VOLUMETRIC_MODEL, volumetricModels.size());
    ospSetParam(g, "volume", OSP_DATA, &d);
    ospRelease(d);
  } else
    ospRemoveParam(g, "volume");

  if (!lights.empty()) {
    auto d = ospNewSharedData1D(lights.data(), OSP_LIGHT, lights.size());
    ospSetParam(g, "light", OSP_DATA, &d);
    ospRelease(d);
  } else
    ospRemoveParam(g, "light");

  m_objectUpdates.lastSceneConstruction = helium::newTimeStamp();
  m_objectUpdates.lastSceneCommit = 0;
  ospraySceneCommit();

  m_geometricModels = std::move(geometricModels);
  m_volumetricModels = std::move(volumetricModels);
  m_lights = std::move(lights);
}

void Group::ospraySceneCommit()
{
  const auto &state = *deviceState();
  if (!m_osprayGroup
      || m_objectUpdates.lastSceneCommit
          > state.objectUpdates.lastBLSCommitSceneRequest)
    return;

  reportMessage(
      ANARI_SEVERITY_DEBUG, "anari_ospray::Group committing ospray group");

  ospCommit(m_osprayGroup);
  m_objectUpdates.lastSceneCommit = helium::newTimeStamp();
}

void Group::cleanup()
{
  m_objectUpdates.lastSceneConstruction = 0;
  m_objectUpdates.lastSceneCommit = 0;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Group *);
