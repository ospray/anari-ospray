// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "array/Array2D.h"

namespace anari_ospray {

Array2D::Array2D(OSPRayGlobalState *state, const Array2DMemoryDescriptor &d)
    : Array(ANARI_ARRAY2D, state, d)
{
  m_size[0] = d.numItems1;
  m_size[1] = d.numItems2;

  initManagedMemory();
}

size_t Array2D::totalSize() const
{
  return size(0) * size(1);
}

size_t Array2D::size(int dim) const
{
  return m_size[dim];
}

uint2 Array2D::size() const
{
  return uint2(uint32_t(size(0)), uint32_t(size(1)));
}

void Array2D::privatize()
{
  makePrivatizedCopy(size(0) * size(1));
}

void Array2D::makeOSPRayDataObject()
{
  releaseOSPRayDataObject();
  m_osprayData = ospNewSharedData(
      data(), enumCast<OSPDataType>(elementType()), size().x, 0, size().y, 0);
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Array2D *);
