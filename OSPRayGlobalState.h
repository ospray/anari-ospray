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
