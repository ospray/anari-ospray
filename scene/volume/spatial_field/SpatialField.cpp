// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "SpatialField.h"
// subtypes
#include "AMRField.h"
#include "StructuredRegularField.h"
#include "UnstructuredField.h"
// std
#include <limits>

namespace anari_ospray {

SpatialField::SpatialField(OSPRayGlobalState *s, const char *osptype)
    : Object(ANARI_SPATIAL_FIELD, s)
{
  m_osprayVolume = ospNewVolume(osptype);
}

SpatialField::~SpatialField()
{
  ospRelease(m_osprayVolume);
}

SpatialField *SpatialField::createInstance(
    std::string_view subtype, OSPRayGlobalState *s)
{
  if (subtype == "structuredRegular")
    return new StructuredRegularField(s);
  else if (subtype == "amr")
    return new AMRField(s);
  else if (subtype == "unstructured")
    return new UnstructuredField(s);
  else
    return (SpatialField *)new UnknownObject(ANARI_SPATIAL_FIELD, s);
}

OSPVolume SpatialField::osprayVolume() const
{
  return m_osprayVolume;
}

template <int T>
constexpr float anariTypeMax()
{
  using Base = typename anari::ANARITypeProperties<T>::base_type;
  return static_cast<float>(std::numeric_limits<Base>::max());
}

// ANARI normalizes ufixed to [0,1] and fixed to [-1,1], but OSPRay reads the
// raw integers; scale the value range by the type's max to match.
float SpatialField::valueRangeScale() const
{
  switch (elementType()) {
  case ANARI_UFIXED8:
    return anariTypeMax<ANARI_UFIXED8>();
  case ANARI_FIXED16:
    return anariTypeMax<ANARI_FIXED16>();
  case ANARI_UFIXED16:
    return anariTypeMax<ANARI_UFIXED16>();
  default:
    return 1.f;
  }
}

ANARIDataType SpatialField::elementType() const
{
  return ANARI_UNKNOWN;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::SpatialField *);
