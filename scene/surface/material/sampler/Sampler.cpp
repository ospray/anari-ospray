// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"
// subtypes
#include "Image1D.h"
#include "Image2D.h"

namespace anari_ospray {

Sampler::Sampler(OSPRayGlobalState *s) : Object(ANARI_SAMPLER, s)
{
  s->objectCounts.samplers++;
}

Sampler::~Sampler()
{
  ospRelease(m_osprayTexture);
  deviceState()->objectCounts.samplers--;
}

Sampler *Sampler::createInstance(std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "image1D")
    return new Image1D(s);
  else if (subtype == "image2D")
    return new Image2D(s);
  else
    return (Sampler *)new UnknownObject(ANARI_SAMPLER, s);
}

OSPTexture Sampler::osprayTexture() const
{
  return m_osprayTexture;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Sampler *);
