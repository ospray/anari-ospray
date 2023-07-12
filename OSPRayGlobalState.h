// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ospray_math.h"
// helium
#include "helium/BaseGlobalDeviceState.h"
// ospray
#include "ospray/ospray.h"
#include "ospray/ospray_util.h"

#include "enumCast.hpp"

namespace anari_ospray {

struct Frame;

struct OSPRayGlobalState : public helium::BaseGlobalDeviceState
{
  struct ObjectCounts
  {
    size_t frames{0};
    size_t cameras{0};
    size_t renderers{0};
    size_t worlds{0};
    size_t instances{0};
    size_t groups{0};
    size_t lights{0};
    size_t surfaces{0};
    size_t geometries{0};
    size_t materials{0};
    size_t samplers{0};
    size_t volumes{0};
    size_t spatialFields{0};
    size_t arrays{0};
    size_t unknown{0};
  } objectCounts;

  struct ObjectUpdates
  {
    helium::TimeStamp lastBLSReconstructSceneRequest{0};
    helium::TimeStamp lastBLSCommitSceneRequest{0};
    helium::TimeStamp lastTLSReconstructSceneRequest{0};
  } objectUpdates;

  Frame *currentFrame{nullptr};

  OSPDevice osprayDevice{nullptr};

  bool allowInvalidSurfaceMaterials{true};
  float4 invalidMaterialColor{1.f, 0.f, 1.f, 1.f};

  // Helper methods //

  OSPRayGlobalState(ANARIDevice d);
  void waitOnCurrentFrame() const;
};

} // namespace anari_ospray
