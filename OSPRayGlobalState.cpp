// Copyright 2023 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "OSPRayGlobalState.h"
#include "frame/Frame.h"

namespace anari_ospray {

OSPRayGlobalState::OSPRayGlobalState(ANARIDevice d)
    : helium::BaseGlobalDeviceState(d)
{}

void OSPRayGlobalState::waitOnCurrentFrame() const
{
  if (currentFrame)
    currentFrame->wait();
}

} // namespace anari_ospray