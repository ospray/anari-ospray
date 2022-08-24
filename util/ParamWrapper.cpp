// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "ParamWrapper.hpp"
#include "../Object.hpp"
// anari
#include "anari/anari_cpp/Traits.h"
#include "anari/anari_enums.h"
#include "anari/type_utility.h"
// std
#include <cstring>
#include <stdexcept>

namespace anari {
namespace ospray {

ParamWrapper::ParamWrapper(std::string _id, int _type, const void *_mem)
    : m_id(_id), m_type(_type)
{
  if (m_mem.size() < anari::sizeOf(type()))
    throw std::runtime_error("parameter too large");

  std::memcpy(m_mem.data(), _mem, anari::sizeOf(type()));
  if (anari::isObject(type()))
    (*(Object **)mem())->refInc();
}

ParamWrapper::~ParamWrapper()
{
  if (anari::isObject(type()))
    (*(Object **)mem())->refDec();
}

ParamWrapper::ParamWrapper(const ParamWrapper &copy)
    : ParamWrapper(copy.id(), copy.type(), copy.mem())
{}

const char *ParamWrapper::id() const
{
  return m_id.c_str();
}

int ParamWrapper::type() const
{
  return m_type;
}

const void *ParamWrapper::mem() const
{
  return m_mem.data();
}

} // namespace ospray
} // namespace anari
