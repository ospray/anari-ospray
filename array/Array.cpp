// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "array/Array.h"
// std
#include <algorithm>

namespace anari_ospray {

// Helper functions //

template <int T>
struct convert_toFloat4
{
  using base_type = typename anari::ANARITypeProperties<T>::base_type;
  const int nc = anari::ANARITypeProperties<T>::components;
  float4 operator()(const void *src, size_t offset)
  {
    float4 retval = {0.f, 0.f, 0.f, 1.f};
    if constexpr (!anari::isObject(T) && T != ANARI_UNKNOWN)
      anari::ANARITypeProperties<T>::toFloat4(
          &retval[0], (const base_type *)src + nc * offset);
    return retval;
  }
};

template <typename T>
static void zeroOutStruct(T &v)
{
  std::memset(&v, 0, sizeof(T));
}

// Array //

Array::Array(ANARIDataType type,
    OSPRayGlobalState *state,
    const ArrayMemoryDescriptor &d)
    : helium::BaseArray(type, state), m_elementType(d.elementType)
{
  state->objectCounts.arrays++;

  if (d.appMemory) {
    m_ownership =
        d.deleter ? ArrayDataOwnership::CAPTURED : ArrayDataOwnership::SHARED;
  } else
    m_ownership = ArrayDataOwnership::MANAGED;

  switch (ownership()) {
  case ArrayDataOwnership::SHARED:
    m_hostData.shared.mem = d.appMemory;
    break;
  case ArrayDataOwnership::CAPTURED:
    m_hostData.captured.mem = d.appMemory;
    m_hostData.captured.deleter = d.deleter;
    m_hostData.captured.deleterPtr = d.deleterPtr;
    break;
  default:
    break;
  }
}

Array::~Array()
{
  freeAppMemory();
  releaseOSPRayDataObject();
  deviceState()->objectCounts.arrays--;
}

ANARIDataType Array::elementType() const
{
  return m_elementType;
}

ArrayDataOwnership Array::ownership() const
{
  return m_ownership;
}

void *Array::data() const
{
  switch (ownership()) {
  case ArrayDataOwnership::SHARED:
    return const_cast<void *>(
        wasPrivatized() ? m_hostData.privatized.mem : m_hostData.shared.mem);
    break;
  case ArrayDataOwnership::CAPTURED:
    return const_cast<void *>(m_hostData.captured.mem);
    break;
  case ArrayDataOwnership::MANAGED:
    return const_cast<void *>(m_hostData.managed.mem);
    break;
  default:
    break;
  }

  return nullptr;
}

size_t Array::totalCapacity() const
{
  return totalSize();
}

bool Array::getProperty(
    const std::string_view &name, ANARIDataType type, void *ptr, uint32_t flags)
{
  return 0;
}

void Array::commit()
{
  // no-op
}

void *Array::map()
{
  if (m_mapped) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "array mapped again without being previously unmapped");
  }
  m_mapped = true;
  deviceState()->waitOnCurrentFrame();
  return data();
}

void Array::unmap()
{
  if (!m_mapped) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "array unmapped again without being previously mapped");
    return;
  }
  m_mapped = false;
  makeOSPRayDataObject();
  notifyCommitObservers();
}

bool Array::wasPrivatized() const
{
  return m_privatized;
}

OSPData Array::osprayData()
{
  if (!m_osprayData)
    makeOSPRayDataObject();
  return m_osprayData;
}

OSPRayGlobalState *Array::deviceState() const
{
  return (OSPRayGlobalState *)helium::BaseObject::m_state;
}

void Array::makePrivatizedCopy(size_t numElements)
{
  if (ownership() != ArrayDataOwnership::SHARED)
    return;

  if (!anari::isObject(elementType())) {
    reportMessage(ANARI_SEVERITY_PERFORMANCE_WARNING,
        "making private copy of shared array (type '%s') | ownership: (%i:%i)",
        anari::toString(elementType()),
        this->useCount(helium::RefType::PUBLIC),
        this->useCount(helium::RefType::INTERNAL));

    size_t numBytes = numElements * anari::sizeOf(elementType());
    m_hostData.privatized.mem = malloc(numBytes);
    std::memcpy(m_hostData.privatized.mem, m_hostData.shared.mem, numBytes);
  }

  m_privatized = true;
  zeroOutStruct(m_hostData.shared);
}

void Array::freeAppMemory()
{
  if (ownership() == ArrayDataOwnership::CAPTURED) {
    auto &captured = m_hostData.captured;
    reportMessage(ANARI_SEVERITY_DEBUG, "invoking array deleter");
    if (captured.deleter)
      captured.deleter(captured.deleterPtr, captured.mem);
    zeroOutStruct(captured);
  } else if (ownership() == ArrayDataOwnership::MANAGED) {
    reportMessage(ANARI_SEVERITY_DEBUG, "freeing managed array");
    free(m_hostData.managed.mem);
    zeroOutStruct(m_hostData.managed);
  } else if (wasPrivatized()) {
    free(m_hostData.privatized.mem);
    zeroOutStruct(m_hostData.privatized);
  }
}

void Array::initManagedMemory()
{
  if (m_hostData.managed.mem != nullptr)
    return;

  if (ownership() == ArrayDataOwnership::MANAGED) {
    auto totalBytes = totalSize() * anari::sizeOf(elementType());
    m_hostData.managed.mem = malloc(totalBytes);
    std::memset(data(), 0, totalBytes);
  }
}

void Array::makeOSPRayDataObject()
{
  // no-op
}

void Array::releaseOSPRayDataObject()
{
  ospRelease(m_osprayData);
  m_osprayData = nullptr;
}

std::vector<float4> convertToColorArray(const Array &arr)
{
  std::vector<float4> retval(arr.totalSize());
  auto type = arr.elementType();
  auto *src = arr.data();
  auto *dst = retval.data();
  size_t i = 0;
  std::for_each(dst, dst + arr.totalSize(), [&](float4 &v) {
    v = anari::anariTypeInvoke<float4, convert_toFloat4>(type, src, i);
    i++;
  });
  return retval;
}

std::vector<float2> convertToTexcoordArray(const Array &arr)
{
  std::vector<float2> retval(arr.totalSize());
  auto type = arr.elementType();
  auto *src = arr.data();
  auto *dst = retval.data();
  size_t i = 0;
  std::for_each(dst, dst + arr.totalSize(), [&](float2 &v) {
    auto r = anari::anariTypeInvoke<float4, convert_toFloat4>(type, src, i);
    v[0] = r.x;
    v[1] = r.y;
    i++;
  });
  return retval;
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::Array *);
