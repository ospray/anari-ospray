// Copyright 2022 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "array/ObjectArray.h"

namespace anari_ospray {

// Helper functions ///////////////////////////////////////////////////////////

static void refIncObject(Object *obj)
{
  if (obj)
    obj->refInc(helium::RefType::INTERNAL);
}

static void refDecObject(Object *obj)
{
  if (obj)
    obj->refDec(helium::RefType::INTERNAL);
}

// ObjectArray definitions ////////////////////////////////////////////////////

ObjectArray::ObjectArray(
    OSPRayGlobalState *state, const Array1DMemoryDescriptor &d)
    : Array(ANARI_ARRAY1D, state, d), m_capacity(d.numItems), m_end(d.numItems)
{
  m_appHandles.resize(d.numItems, nullptr);
  initManagedMemory();
  updateInternalHandleArrays();
}

ObjectArray::~ObjectArray()
{
  std::for_each(m_appHandles.begin(), m_appHandles.end(), refDecObject);
  std::for_each(
      m_appendedHandles.begin(), m_appendedHandles.end(), refDecObject);
}

void ObjectArray::commitParameters()
{
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
}

void ObjectArray::finalize()
{
  markDataModified();
  notifyChangeObservers();
}

size_t ObjectArray::totalSize() const
{
  return size() + m_appendedHandles.size();
}

size_t ObjectArray::totalCapacity() const
{
  return m_capacity;
}

size_t ObjectArray::size() const
{
  return m_end - m_begin;
}

void ObjectArray::unmap()
{
  if (isMapped())
    updateInternalHandleArrays();
  Array::unmap();
}

Object **ObjectArray::handlesBegin() const
{
  return m_liveHandles.data() + m_begin;
}

Object **ObjectArray::handlesEnd() const
{
  return handlesBegin() + totalSize();
}

void ObjectArray::appendHandle(Object *o)
{
  o->refInc(helium::RefType::INTERNAL);
  m_appendedHandles.push_back(o);
  updateInternalHandleArrays();
}

void ObjectArray::removeAppendedHandles()
{
  m_liveHandles.resize(size());
  for (auto o : m_appendedHandles)
    o->refDec(helium::RefType::INTERNAL);
  m_appendedHandles.clear();
}

void ObjectArray::updateInternalHandleArrays() const
{
  m_liveHandles.resize(totalSize());

  if (data()) {
    auto **srcAllBegin = (Object **)data();
    auto **srcAllEnd = srcAllBegin + totalCapacity();
    std::for_each(srcAllBegin, srcAllEnd, refIncObject);
    std::for_each(m_appHandles.begin(), m_appHandles.end(), refDecObject);
    std::copy(srcAllBegin, srcAllEnd, m_appHandles.data());

    auto **srcRegionBegin = srcAllBegin + m_begin;
    auto **srcRegionEnd = srcRegionBegin + size();
    std::copy(srcRegionBegin, srcRegionEnd, m_liveHandles.data());
  }

  std::copy(m_appendedHandles.begin(),
      m_appendedHandles.end(),
      m_liveHandles.begin() + size());
}

void ObjectArray::privatize()
{
  makePrivatizedCopy(size());
  freeAppMemory();
  if (data()) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "ObjectArray privatized but host array still present");
  }
}

} // namespace anari_ospray

OSPRAY_ANARI_TYPEFOR_DEFINITION(anari_ospray::ObjectArray *);
