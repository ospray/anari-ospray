// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Volume.h"
// subtypes
#include "TransferFunction1D.h"

namespace anari_ospray {

Volume::Volume(OSPRayGlobalState *s) : Object(ANARI_VOLUME, s)
{
  s->objectCounts.volumes++;
  m_osprayModel = ospNewVolumetricModel();
}

Volume::~Volume()
{
  ospRelease(m_osprayModel);
  deviceState()->objectCounts.volumes--;
}

Volume *Volume::createInstance(std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "transferFunction1D" || subtype == "scivis")
    return new TransferFunction1DVolume(s);
  else
    return (Volume *)new UnknownObject(ANARI_VOLUME, s);
}

OSPVolumetricModel Volume::osprayModel() const
{
  return m_osprayModel;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Volume *);
