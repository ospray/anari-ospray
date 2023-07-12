// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.h"
// subtypes
#include "AMRField.h"
#include "StructuredRegularField.h"

namespace anari_ospray {

SpatialField::SpatialField(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_SPATIAL_FIELD, s)
{
  s->objectCounts.spatialFields++;
  m_osprayVolume = ospNewVolume(osptype);
}

SpatialField::~SpatialField()
{
  ospRelease(m_osprayVolume);
  deviceState()->objectCounts.spatialFields--;
}

SpatialField *SpatialField::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "structuredRegular")
    return new StructuredRegularField(s);
  else if (subtype == "amr")
    return new AMRField(s);
  else
    return (SpatialField *)new UnknownObject(ANARI_SPATIAL_FIELD, s);
}

OSPVolume SpatialField::osprayVolume() const
{
  return m_osprayVolume;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::SpatialField *);
