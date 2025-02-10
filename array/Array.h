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

  const void *data() const;

  template <typename T>
  const T *dataAs() const;

  virtual size_t totalSize() const = 0;
  virtual size_t totalCapacity() const;

  virtual void *map() override;
  virtual void unmap() override;

  bool isMapped() const;

  bool wasPrivatized() const;

  void markDataModified();

  virtual bool getProperty(const std::string_view &name,
      ANARIDataType type,
      void *ptr,
      uint32_t flags) override;
  virtual void commitParameters() override;
  virtual void finalize() override;

  OSPData osprayData();

  // CONSOLIDATE INTO anari_ospray::Object/////////////////////////////////////
  OSPRayGlobalState *deviceState() const;
  /////////////////////////////////////////////////////////////////////////////

 protected:
  virtual void privatize() override = 0;

  void makePrivatizedCopy(size_t numElements);
  void freeAppMemory();
  void initManagedMemory();

  template <typename T>
  void throwIfDifferentElementType() const;

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

  helium::TimeStamp m_lastDataModified{0};
  bool m_mapped{false};
  OSPData m_osprayData{nullptr};

 private:
  void on_NoPublicReferences() override;

  ArrayDataOwnership m_ownership{ArrayDataOwnership::INVALID};
  ANARIDataType m_elementType{ANARI_UNKNOWN};
  bool m_privatized{false};
};

std::vector<float4> convertToColorArray(const Array &arr);
std::vector<float2> convertToTexcoordArray(const Array &arr);

// Inlined definitions ////////////////////////////////////////////////////////

template <typename T>
inline const T *Array::dataAs() const
{
  throwIfDifferentElementType<T>();
  return (const T *)data();
}

template <typename T>
inline void Array::throwIfDifferentElementType() const
{
  constexpr auto t = anari::ANARITypeFor<T>::value;
  static_assert(
      t != ANARI_UNKNOWN, "unknown type used to query array element type");

  if (t != elementType()) {
    std::stringstream msg;
    msg << "incorrect element type queried for array -- asked for '"
        << anari::toString(t) << "', but array stores '"
        << anari::toString(elementType()) << "'";
    throw std::runtime_error(msg.str());
  }
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_SPECIALIZATION(anari_ospray::Array *, ANARI_ARRAY);
