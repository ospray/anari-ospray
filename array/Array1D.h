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

  void commitParameters() override;
  void finalize() override;

  size_t totalSize() const override;
  size_t totalCapacity() const override;

  const void *begin() const;
  const void *end() const;

  template <typename T>
  const T *beginAs() const;
  template <typename T>
  const T *endAs() const;

  size_t size() const;

 private:
  size_t m_capacity{0};
  size_t m_begin{0};
  size_t m_end{0};

  void privatize() override;
  void makeOSPRayDataObject() override;
};

// Inlined definitions ////////////////////////////////////////////////////////

template <typename T>
inline const T *Array1D::beginAs() const
{
  return dataAs<T>() + m_begin;
}

template <typename T>
inline const T *Array1D::endAs() const
{
  return dataAs<T>() + m_end;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Array1D *, ANARI_ARRAY1D);
