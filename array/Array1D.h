// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "array/Array.h"

namespace anari_ospray {

struct Array1DMemoryDescriptor : public ArrayMemoryDescriptor
{
  uint64_t numItems{0};
};

bool isCompact(const Array1DMemoryDescriptor &d);

struct Array1D : public Array
{
  Array1D(OSPRayGlobalState *state, const Array1DMemoryDescriptor &d);

  void commit() override;

  size_t totalSize() const override;
  size_t totalCapacity() const override;

  void *begin() const;
  void *end() const;

  template <typename T>
  T *beginAs() const;
  template <typename T>
  T *endAs() const;

  size_t size() const;

  void privatize() override;

 private:
  void makeOSPRayDataObject() override;

  size_t m_capacity{0};
  size_t m_begin{0};
  size_t m_end{0};
};

// Inlined definitions ////////////////////////////////////////////////////////

template <typename T>
inline T *Array1D::beginAs() const
{
  if (anari::ANARITypeFor<T>::value != elementType())
    throw std::runtime_error("incorrect element type queried for array");

  return (T *)data() + m_begin;
}

template <typename T>
inline T *Array1D::endAs() const
{
  if (anari::ANARITypeFor<T>::value != elementType())
    throw std::runtime_error("incorrect element type queried for array");

  return (T *)data() + m_end;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Array1D *, ANARI_ARRAY1D);
