// Copyright 2021 Intel Corporation
// Copyright 2021 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Group.hpp"

namespace anari {
namespace ospray {

Group::Group() : Object(ospNewGroup(), "<none>", CommitPriority::GROUP) {}

void Group::setParam(const char *_id, ANARIDataType type, const void *mem)
{
  std::string id(_id);

  if (id == "surface")
    id = "geometry";

  Object::setParam(id.c_str(), type, mem);
}
} // namespace ospray
} // namespace anari
