// Copyright 2020 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "Array.hpp"
// std
#include <algorithm>
#include <cstring>
#include <vector>

namespace anari {
namespace ospray {

Array::Array(void *appMem,
    size_t count1,
    size_t count2,
    size_t count3,
    ANARIMemoryDeleter deleter,
    void *deleterPtr,
    ANARIDataType elementType)
    : count1(count1), count2(count2), count3(count3)
{
  std::vector<OSPObject> tmpHandlesBuffer;
  OSPData tmp{nullptr};
  OSPData data =
      ospNewData(enumCast<OSPDataType>(elementType), count1, count2, count3);

  if (anari::isObject(elementType)) {
    tmpHandlesBuffer.resize(count1);
    auto &tp = tmpHandlesBuffer;
    auto **begin = (Object **)appMem;
    auto **end = begin + count1;
    std::transform(
        begin, end, tp.begin(), [](Object *obj) { return obj->handle(); });
    tmp =
        ospNewSharedData(tp.data(), enumCast<OSPDataType>(elementType), count1);
  } else {
    tmp = ospNewSharedData(appMem,
        enumCast<OSPDataType>(elementType),
        count1,
        0,
        count2,
        0,
        count3,
        0);
  }

  ospCopyData(tmp, data);
  ospRelease(tmp);

  m_object = data;

  if (deleter)
    deleter(deleterPtr, appMem);
}

} // namespace ospray
} // namespace anari
