// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "array/Array1D.h"

namespace anari_ospray {

Array1D::Array1D(OSPRayGlobalState *state, const Array1DMemoryDescriptor &d)
    : Array(ANARI_ARRAY1D, state, d), m_capacity(d.numItems), m_end(d.numItems)
{
  initManagedMemory();
}

void Array1D::commit()
{
  auto oldBegin = m_begin;
  auto oldEnd = m_end;

  m_begin = getParam<size_t>("begin", 0);
  m_begin = std::clamp(m_begin, size_t(0), m_capacity - 1);
  m_end = getParam<size_t>("end", m_capacity);
  m_end = std::clamp(m_end, size_t(1), m_capacity);

  if (size() == 0) {
    reportMessage(ANARI_SEVERITY_ERROR, "array size must be greater than zero");
    return;
  }

  if (m_begin > m_end) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "array 'begin' is not less than 'end', swapping values");
    std::swap(m_begin, m_end);
  }

  if (m_begin != oldBegin || m_end != oldEnd)
    notifyChangeObservers();
}

size_t Array1D::totalSize() const
{
  return size();
}

size_t Array1D::totalCapacity() const
{
  return m_capacity;
}

void *Array1D::begin() const
{
  auto *p = (unsigned char *)data();
  auto s = anari::sizeOf(elementType());
  return p + (s * m_begin);
}

void *Array1D::end() const
{
  auto *p = (unsigned char *)data();
  auto s = anari::sizeOf(elementType());
  return p + (s * m_end);
}

size_t Array1D::size() const
{
  return m_end - m_begin;
}

void Array1D::privatize()
{
  makePrivatizedCopy(size());
}

void Array1D::makeOSPRayDataObject()
{
  releaseOSPRayDataObject();
  m_osprayData = ospNewSharedData(
      data(), enumCast<OSPDataType>(elementType()), totalCapacity());
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Array1D *);
