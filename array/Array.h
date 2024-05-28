// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Object.h"
// helium
#include "helium/BaseArray.h"
// anari
#include "anari/anari_cpp.hpp"

namespace anari_ospray {

enum class ArrayDataOwnership
{
  SHARED,
  CAPTURED,
  MANAGED,
  INVALID
};

struct ArrayMemoryDescriptor
{
  const void *appMemory{nullptr};
  ANARIMemoryDeleter deleter{};
  const void *deleterPtr{nullptr};
  ANARIDataType elementType{ANARI_UNKNOWN};
};

struct Array : public helium::BaseArray
{
  Array(ANARIDataType type,
      OSPRayGlobalState *state,
      const ArrayMemoryDescriptor &d);
  virtual ~Array();

  ANARIDataType elementType() const;
  ArrayDataOwnership ownership() const;

  void *data() const;

  template <typename T>
  T *dataAs() const;

  virtual size_t totalSize() const = 0;
  virtual size_t totalCapacity() const;

  bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;
  void commit() override;
  void *map() override;
  virtual void unmap() override;
  virtual void privatize() override = 0;

  bool wasPrivatized() const;

  OSPData osprayData();

  // CONSOLIDATE INTO anari_ospray::Object/////////////////////////////////////
  OSPRayGlobalState *deviceState() const;
  /////////////////////////////////////////////////////////////////////////////

 protected:
  void makePrivatizedCopy(size_t numElements);
  void freeAppMemory();
  void initManagedMemory();

  virtual void makeOSPRayDataObject();
  void releaseOSPRayDataObject();

  struct ArrayDescriptor
  {
    struct SharedData
    {
      const void *mem{nullptr};
    } shared;

    struct CapturedData
    {
      const void *mem{nullptr};
      ANARIMemoryDeleter deleter{nullptr};
      const void *deleterPtr{nullptr};
    } captured;

    struct ManagedData
    {
      void *mem{nullptr};
    } managed;

    struct PrivatizedData
    {
      void *mem{nullptr};
    } privatized;
  } m_hostData;

  bool m_mapped{false};
  OSPData m_osprayData{nullptr};

 private:
  ArrayDataOwnership m_ownership{ArrayDataOwnership::INVALID};
  ANARIDataType m_elementType{ANARI_UNKNOWN};
  bool m_privatized{false};
  mutable bool m_usedOnDevice{false};
};

std::vector<float4> convertToColorArray(const Array &arr);
std::vector<float2> convertToTexcoordArray(const Array &arr);

// Inlined definitions ////////////////////////////////////////////////////////

template <typename T>
inline T *Array::dataAs() const
{
  if (anari::ANARITypeFor<T>::value != m_elementType)
    throw std::runtime_error("incorrect element type queried for array");

  return (T *)data();
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Array *, ANARI_ARRAY);
